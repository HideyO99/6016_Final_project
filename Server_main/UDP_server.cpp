#include "UDP_server.h"

UDP_server::UDP_server()
{
}

UDP_server::~UDP_server()
{
}

int UDP_server::initialize()
{
	WSADATA wsaData_;
	int result;

	result = winsockInit();
	result = socketCreate();
	result = bindSocket();

	return result;
}

int UDP_server::closeWinsock()
{
	closesocket(udpSocket);
	WSACleanup();

	return 0;
}

int UDP_server::winsockInit()
{
	WSADATA wsaData_;
	int result;

	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons(PORT);
	recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	result = WSAStartup(MAKEWORD(2, 2), &wsaData_);
	if (result != 0)
	{
		std::cout << "cannot startup WSA" << std::endl;
		return 1;
	}
	else
	{
		std::cout << " WSAStartup success" << std::endl;
	}

	return 0;
}

int UDP_server::socketCreate()
{
	int result;
	udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpSocket == INVALID_SOCKET) {
		std::cout << "Socket error:" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 2;
	}
	else
	{
		std::cout << "Create Socket success" << std::endl;
	}

	DWORD NonBlock = 1;
	result = ioctlsocket(udpSocket, FIONBIO, &NonBlock);
	if (result != 0)
	{
		std::cout << "IOCTL Socket error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 3;
	}
	else
	{
		std::cout << "Create IOCTL Socket success" << std::endl;
	}

	return 0;
}

int UDP_server::bindSocket()
{
	int result;

	result = bind(udpSocket, (SOCKADDR*)&recvAddr, sizeof(recvAddr));
	if (result != 0) {
		std::cout << "Binding Socket error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 4;
	}
	else
	{
		std::cout << "Binding success" << std::endl;
	}

	clientInfo.clientAddrSize = sizeof(clientInfo.clientAddr);
	clientInfo.HaveInfo = false;
	
	return 0;
}
