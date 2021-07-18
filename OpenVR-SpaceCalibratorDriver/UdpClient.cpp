#pragma once

#include "UdpClient.h"
#include "Logging.h"

#include <ws2tcpip.h>

UdpClient::UdpClient()
{
}

UdpClient::~UdpClient()
{
	Close();
}

void UdpClient::Connect()
{
    WSADATA wsaData;
    auto rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc) {
        LOG("WSAStartup Failed.\n");
        return;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
}

void UdpClient::Send(std::string str)
{
    sendto(sock, str.c_str(), str.size(), 0, (struct sockaddr*)&addr, sizeof(addr));
}

void UdpClient::Close()
{
    closesocket(sock);
	WSACleanup();
}