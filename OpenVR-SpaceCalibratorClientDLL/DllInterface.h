#pragma once

#include "../Protocol.h"
#include "IPCClient.h"

extern "C"
{
	__declspec(dllexport) void Connect(char* error);
	__declspec(dllexport) void SetDeviceTransform(uint32_t id, double px, double py, double pz, double qx, double qy, double qz, double qw, double scale);
	__declspec(dllexport) void ResetAndDisableDeviceTransform(uint32_t id);
	__declspec(dllexport) protocol::DevicePoses GetDevicePoses();
}
