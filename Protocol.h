#pragma once

#define _WINSOCKAPI_

#include <cstdint>

#ifndef _OPENVR_API
#include <openvr_driver.h>
#endif

#define OPENVR_SPACECALIBRATOR_PIPE_NAME "\\\\.\\pipe\\OpenVRSpaceCalibratorDriver"

namespace protocol
{
	const uint32_t Version = 2;

	enum RequestType
	{
		RequestInvalid,
		RequestHandshake,
		RequestSetDeviceTransform,
		RequestGetDevicePoses,
	};

	enum ResponseType
	{
		ResponseInvalid,
		ResponseHandshake,
		ResponseSuccess,
		ResponseDevicePoses,
	};

	struct Protocol
	{
		uint32_t version = Version;
	};

	struct SetDeviceTransform
	{
		uint32_t openVRID;
		bool enabled;
		bool updateTranslation;
		bool updateRotation;
		bool updateScale;
		vr::HmdVector3d_t translation;
		vr::HmdQuaternion_t rotation;
		double scale;

		SetDeviceTransform(uint32_t id, bool enabled) :
			openVRID(id), enabled(enabled), updateTranslation(false), updateRotation(false), updateScale(false) { }

		SetDeviceTransform(uint32_t id, bool enabled, vr::HmdVector3d_t translation) :
			openVRID(id), enabled(enabled), updateTranslation(true), updateRotation(false), updateScale(false), translation(translation) { }

		SetDeviceTransform(uint32_t id, bool enabled, vr::HmdQuaternion_t rotation) :
			openVRID(id), enabled(enabled), updateTranslation(false), updateRotation(true), updateScale(false), rotation(rotation) { }

		SetDeviceTransform(uint32_t id, bool enabled, double scale) :
			openVRID(id), enabled(enabled), updateTranslation(false), updateRotation(false), updateScale(true), scale(scale) { }

		SetDeviceTransform(uint32_t id, bool enabled, vr::HmdVector3d_t translation, vr::HmdQuaternion_t rotation) :
			openVRID(id), enabled(enabled), updateTranslation(true), updateRotation(true), updateScale(false), translation(translation), rotation(rotation) { }

		SetDeviceTransform(uint32_t id, bool enabled, vr::HmdVector3d_t translation, vr::HmdQuaternion_t rotation, double scale) :
			openVRID(id), enabled(enabled), updateTranslation(true), updateRotation(true), updateScale(true), translation(translation), rotation(rotation), scale(scale) { }
	};

	struct Request
	{
		RequestType type;

		union {
			SetDeviceTransform setDeviceTransform;
		};

		Request() : type(RequestInvalid) { }
		Request(RequestType type) : type(type) { }
	};

	struct DevicePoses
	{
		struct DevicePose
		{
			uint32_t openVRID;

			vr::HmdQuaternion_t qWorldFromDriverRotation;
			double vecWorldFromDriverTranslation[3];

			vr::HmdQuaternion_t qDriverFromHeadRotation;
			double vecDriverFromHeadTranslation[3];

			double vecPosition[3];
			vr::HmdQuaternion_t qRotation;
		};

		uint32_t length;
		DevicePose devicePoses[8];
	};

	struct Response
	{
		ResponseType type;

		union {
			Protocol protocol;
			DevicePoses devicePoses;
		};

		Response() : type(ResponseInvalid) { }
		Response(ResponseType type) : type(type) { }
	};
}