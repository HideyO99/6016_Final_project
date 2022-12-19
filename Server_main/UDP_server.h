#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#define PORT 7777

class UDP_server
{
public:
	UDP_server();
	~UDP_server();

	struct sClientInfo {
		bool HaveInfo;
		sockaddr_in clientAddr;
		int clientAddrSize;
		int playerID;
	};


	SOCKET udpSocket;
	sockaddr_in recvAddr;
	sClientInfo clientInfo;

	int initialize();
	int closeWinsock();
private:
	int winsockInit();
	int socketCreate();
	int bindSocket();
};

