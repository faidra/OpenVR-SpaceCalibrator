#pragma once

#include "../Protocol.h"
#include "IPCClient.h"

extern "C"
{
	__declspec(dllexport) void Connect();
	__declspec(dllexport) void SetDeviceOffset(uint32_t id, double x, double y, double z);
	__declspec(dllexport) void ResetAndDisableOffsets(uint32_t id);
}
