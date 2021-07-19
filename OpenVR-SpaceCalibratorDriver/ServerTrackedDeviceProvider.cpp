#include "ServerTrackedDeviceProvider.h"
#include "Logging.h"
#include "InterfaceHookInjector.h"

#include <string>
#include <sstream>

vr::EVRInitError ServerTrackedDeviceProvider::Init(vr::IVRDriverContext *pDriverContext)
{
	TRACE("ServerTrackedDeviceProvider::Init()");
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

	memset(transforms, 0, vr::k_unMaxTrackedDeviceCount * sizeof DeviceTransform);

	InjectHooks(this, pDriverContext);
	server.Run();
	client.Connect();

	return vr::VRInitError_None;
}

void ServerTrackedDeviceProvider::Cleanup()
{
	TRACE("ServerTrackedDeviceProvider::Cleanup()");
	server.Stop();
	DisableHooks();
	VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

inline vr::HmdQuaternion_t operator*(const vr::HmdQuaternion_t &lhs, const vr::HmdQuaternion_t &rhs) {
	return {
		(lhs.w * rhs.w) - (lhs.x * rhs.x) - (lhs.y * rhs.y) - (lhs.z * rhs.z),
		(lhs.w * rhs.x) + (lhs.x * rhs.w) + (lhs.y * rhs.z) - (lhs.z * rhs.y),
		(lhs.w * rhs.y) + (lhs.y * rhs.w) + (lhs.z * rhs.x) - (lhs.x * rhs.z),
		(lhs.w * rhs.z) + (lhs.z * rhs.w) + (lhs.x * rhs.y) - (lhs.y * rhs.x)
	};
}

inline vr::HmdVector3d_t quaternionRotateVector(const vr::HmdQuaternion_t& quat, const double(&vector)[3]) {
	vr::HmdQuaternion_t vectorQuat = { 0.0, vector[0], vector[1] , vector[2] };
	vr::HmdQuaternion_t conjugate = { quat.w, -quat.x, -quat.y, -quat.z };
	auto rotatedVectorQuat = quat * vectorQuat * conjugate;
	return { rotatedVectorQuat.x, rotatedVectorQuat.y, rotatedVectorQuat.z };
}

void ServerTrackedDeviceProvider::SetDeviceTransform(const protocol::SetDeviceTransform &newTransform)
{
	auto &tf = transforms[newTransform.openVRID];
	tf.enabled = newTransform.enabled;

	if (newTransform.updateTranslation)
		tf.translation = newTransform.translation;

	if (newTransform.updateRotation)
		tf.rotation = newTransform.rotation;
}

bool ServerTrackedDeviceProvider::HandleDevicePoseUpdated(uint32_t openVRID, vr::DriverPose_t &pose)
{
	auto &tf = transforms[openVRID];
	if (tf.enabled)
	{
		pose.qWorldFromDriverRotation = tf.rotation * pose.qWorldFromDriverRotation;

		vr::HmdVector3d_t rotatedTranslation = quaternionRotateVector(tf.rotation, pose.vecWorldFromDriverTranslation);
		pose.vecWorldFromDriverTranslation[0] = rotatedTranslation.v[0] + tf.translation.v[0];
		pose.vecWorldFromDriverTranslation[1] = rotatedTranslation.v[1] + tf.translation.v[1];
		pose.vecWorldFromDriverTranslation[2] = rotatedTranslation.v[2] + tf.translation.v[2];
	}
	return true;
}

void ServerTrackedDeviceProvider::SendPose(uint32_t openVRID, vr::DriverPose_t& pose)
{
	std::stringstream str;
	str << openVRID << ',';
	str << pose.vecPosition[0] << ',';
	str << pose.vecPosition[1] << ',';
	str << pose.vecPosition[2] << ',';
	str << pose.qRotation.x << ',';
	str << pose.qRotation.y << ',';
	str << pose.qRotation.z << ',';
	str << pose.qRotation.w << ',';
	str << pose.vecWorldFromDriverTranslation[0] << ',';
	str << pose.vecWorldFromDriverTranslation[1] << ',';
	str << pose.vecWorldFromDriverTranslation[2] << ',';
	str << pose.qWorldFromDriverRotation.x << ',';
	str << pose.qWorldFromDriverRotation.y << ',';
	str << pose.qWorldFromDriverRotation.z << ',';
	str << pose.qWorldFromDriverRotation.w << ',';
	str << pose.vecDriverFromHeadTranslation[0] << ',';
	str << pose.vecDriverFromHeadTranslation[1] << ',';
	str << pose.vecDriverFromHeadTranslation[2] << ',';
	str << pose.qDriverFromHeadRotation.x << ',';
	str << pose.qDriverFromHeadRotation.y << ',';
	str << pose.qDriverFromHeadRotation.z << ',';
	str << pose.qDriverFromHeadRotation.w << ',';

	client.Send(str.str());
}