#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _WINSOCKAPI_

#include <winsock2.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class UdpClient
{
public:
	UdpClient();
	~UdpClient();

	void Connect();
	void Send(std::string str);

private:
	SOCKET sock;
	sockaddr_in addr;

	void Close();
};