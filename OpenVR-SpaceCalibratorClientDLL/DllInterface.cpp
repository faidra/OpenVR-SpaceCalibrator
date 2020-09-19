#include "stdafx.h"
#include "DllInterface.h"

static IPCClient Driver;

void Connect()
{
	Driver.Connect();
}

void SetDeviceOffset(uint32_t id, double x, double y, double z)
{
	vr::HmdVector3d_t pos;
	pos.v[0] = x;
	pos.v[1] = y;
	pos.v[2] = z;

	vr::HmdQuaternion_t zeroQ;
	zeroQ.x = 0; zeroQ.y = 0; zeroQ.z = 0; zeroQ.w = 1;

	protocol::Request req(protocol::RequestSetDeviceTransform);
	req.setDeviceTransform = { id, true, pos, zeroQ };
	Driver.SendBlocking(req);
}

void ResetAndDisableOffsets(uint32_t id)
{
	vr::HmdVector3d_t zeroV;
	zeroV.v[0] = zeroV.v[1] = zeroV.v[2] = 0;

	vr::HmdQuaternion_t zeroQ;
	zeroQ.x = 0; zeroQ.y = 0; zeroQ.z = 0; zeroQ.w = 1;

	protocol::Request req(protocol::RequestSetDeviceTransform);
	req.setDeviceTransform = { id, false, zeroV, zeroQ };
	Driver.SendBlocking(req);
}
