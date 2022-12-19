#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <chrono>
#include "UDP_server.h"

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "7777"
#define ESC 27

std::chrono::steady_clock::time_point g_LastUpdateTime;
std::chrono::steady_clock::time_point g_CurrentUpdateTime;

float g_CurrentTime = 0;
float g_NextNetworkSend = 0;
float g_SendDeltaTime = 1 / 5.f;
float g_NextNetworkRecv = 0;
float g_RecvDeltaTime = 1 / 5.f;
bool isQuit = false;

struct ServerInfo {
	SOCKET socket;
};

struct PlayerInfo {
	int id = -1;
	float posX, posZ;
	float dirX, dirZ;
	bool isFire;
	bool dead = false;
};

struct GameState {
	PlayerInfo player1;
	PlayerInfo player2;
	PlayerInfo player3;
	PlayerInfo player4;
};

struct ClientInfo {
	bool HaveInfo;
	sockaddr_in clientAddr;
	int clientAddrSize;
	int playerID;
};

struct UserCMD {
	int pID;
	bool W, A, S, D,fire;
};

ServerInfo g_ServerInfo;
int g_Iteration;
GameState g_GameState;
ClientInfo g_ClientInfo;
UserCMD g_UserInput;

void userInput();
void run();
void Update();
void GetInfoFromClients();

int main(int argc, char** argv)
{
	int result;

	UDP_server* udp_server = new UDP_server();

	result = udp_server->initialize(); 

	while (!isQuit)
	{
		userInput();
		run();
	}

	result = udp_server->closeWinsock();

	return result;
}

void userInput()
{
	if (_kbhit())
	{
		int ch = _getch();
		if (ch == ESC)
			isQuit = true;
	}
}

void run()
{
	g_LastUpdateTime = g_CurrentUpdateTime;
	g_CurrentUpdateTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = g_CurrentUpdateTime - g_LastUpdateTime;

	if (elapsed.count() < 100)
		g_CurrentTime += elapsed.count();

	GetInfoFromClients();

	Update();

	UpdateClientsWithGameState();
}

void Update()
{
	// Update the game state
	if (g_UserInput.A) {
		g_GameState.player.x--;
	}
	if (g_UserInput.D) {
		g_GameState.player.x++;
	}
	if (g_UserInput.W) {
		g_GameState.player.z++;
	}
	if (g_UserInput.S) {
		g_GameState.player.z--;
	}
}

void GetInfoFromClients()
{
	if (g_CurrentTime < g_NextNetworkRecv)
		return;
	g_NextNetworkRecv += g_RecvDeltaTime;

	const int bufsize = sizeof(UserCMD);
	char buf[bufsize];

	int recvResult = recvfrom(g_ServerInfo.socket, buf, bufsize, 0, (SOCKADDR*)&g_ClientInfo.clientAddr, &g_ClientInfo.clientAddrSize);
	if (recvResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(g_ServerInfo.socket);
			WSACleanup();
			isQuit = true;
		}

		return;
	}

	g_ClientInfo.HaveInfo = true;
	memcpy(&g_UserInput, (const void*)buf, bufsize);

	printf("%d RECV: %d %d %d %d\n ", g_Iteration++, g_UserInput.W, g_UserInput.A, g_UserInput.S, g_UserInput.D);
}

void UpdateClientsWithGameState()
{
	if (g_ClientInfo.HaveInfo == false)
		return;

	if (g_CurrentTime < g_NextNetworkSend)
		return;
	g_NextNetworkSend += g_SendDeltaTime;

	int gameStateSize = sizeof(GameState);
	printf("SEND: { %.2f, %.2f }\n", g_GameState.player.x, g_GameState.player.z);
	int sendResult = sendto(g_ServerInfo.socket, (const char*)&g_GameState, gameStateSize, 0, (SOCKADDR*)&g_ClientInfo.clientAddr, g_ClientInfo.clientAddrSize);
	if (sendResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(g_ServerInfo.socket);
			WSACleanup();
			isQuit = true;
		}

		return;
	}
}