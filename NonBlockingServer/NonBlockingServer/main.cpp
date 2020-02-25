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

	//Create a server socket

	struct addrinfo* ptr = NULL, hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//Finds a connection from any address
	if (getaddrinfo(INADDR_ANY, "5000", &hints, &ptr) != 0)
	{
		printf("Getaddrinfo failed! %d\n", WSAGetLastError());
		WSACleanup(); //Cleanup the resources we were using
		return 1;
	}

	SOCKET serverSocket;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serverSocket == INVALID_SOCKET)
	{
		printf("Failed creating socket %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//Bind socket

	if (bind(serverSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR)
	{
		printf("Bind failed: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}

	//Listen on socket

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Listen failed: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}

	printf("Waiting for connections...\n");

	//Accept a connection (multiple clients --> threads)

	SOCKET clientSocket;

	clientSocket = accept(serverSocket, NULL, NULL);

	if (clientSocket == INVALID_SOCKET)
	{
		printf("Accept() failed: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}

	printf("Client connected!!\n");

	//Send a message to client

	const unsigned int BUF_LEN = 512;

	char send_buf[BUF_LEN];
	memset(send_buf, 0, BUF_LEN);
	strcpy_s(send_buf, "Welcome to INFR3830 Server!!\r\n");

	if (send(clientSocket, send_buf, BUF_LEN, 0) == SOCKET_ERROR)
	{
		printf("Failed to send message to client %d\n", WSAGetLastError());
		closesocket(clientSocket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}

	printf("Message sent to client\n");


	//Shutdown the socket

	if (shutdown(serverSocket, SD_BOTH) == SOCKET_ERROR)
	{
		printf("Shutdown failed! %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	closesocket(serverSocket);
	freeaddrinfo(ptr);
	WSACleanup();

	return 0;
}