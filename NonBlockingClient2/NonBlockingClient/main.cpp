#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib") //He did a capital 'W'

int main()
{
	//Initialize Winsock

	WSADATA wsa;

	int error;
	error = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (error != 0)
	{
		printf("Failed to intialize %d", error);
		//If failed, nothing to do but return.
		return 1;
	}

	//Create a client socket

	struct addrinfo* ptr = NULL, hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//Instead of "localhost" he did "127.0.0.1" could apparently use either (they are the same??)
	if (getaddrinfo("localhost", "5000", &hints, &ptr) != 0)
	{
		printf("Getaddrinfo failed! %d\n", WSAGetLastError());
		WSACleanup(); //Cleanup the resources we were using
		return 1;
	}

	SOCKET cliSocket;
	cliSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (cliSocket == INVALID_SOCKET)
	{
		printf("Failed creating socket %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//Connect to the server

	if (connect(cliSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR)
	{
		printf("Unable to connect to server: %d\n", WSAGetLastError());
		closesocket(cliSocket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}

	printf("Connected to the server\n");


	//Create a buffer

	const unsigned int BUF_LEN = 512;

	char recv_buf[BUF_LEN];
	memset(recv_buf, 0, BUF_LEN);

	//Change to non-blocking mode
	u_long mode = 1; // 0 is for blocking, 1 is for non blocking
	ioctlsocket(cliSocket, FIONBIO, &mode);

	int sError = -1;
	int bytes_received = -1;

	for (;;) //Infinite loop
	{
		bytes_received = recv(cliSocket, recv_buf, BUF_LEN, 0);
		sError = WSAGetLastError();

		if (sError != WSAEWOULDBLOCK && bytes_received > 0)
		{
			printf("Received from server: %s\n", recv_buf);
			memset(recv_buf, 0, BUF_LEN);
		}
	}

	if (recv(cliSocket, recv_buf, BUF_LEN, 0) > 0)
	{
		printf("Received from server: %s\n", recv_buf);
	}
	else
	{
		printf("recv() Error: %d\n", WSAGetLastError());
	}

	//Shutdown the socket

	if (shutdown(cliSocket, SD_BOTH) == SOCKET_ERROR)
	{
		printf("Shutdown failed! %d\n", WSAGetLastError());
		closesocket(cliSocket);
		WSACleanup();
		return 1;
	}

	closesocket(cliSocket);
	freeaddrinfo(ptr);
	WSACleanup();

	return 0;
}