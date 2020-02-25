//UDP Client
//Gil Robern - 100651824
//Jessica Le - 100555079
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
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	//Instead of "localhost" he did "127.0.0.1" could apparently use either (they are the same??)
	if (getaddrinfo("localhost", "8888", &hints, &ptr) != 0)
	{
		printf("Getaddrinfo failed! %d\n", WSAGetLastError());
		WSACleanup(); //Cleanup the resources we were using
		return 1;
	}

	SOCKET cliSocket;
	cliSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (cliSocket == INVALID_SOCKET)
	{
		printf("Failed creating socket %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//Create a buffer

	const unsigned int BUF_LEN = 512;

	char send_buf[BUF_LEN];
	memset(send_buf, 0, BUF_LEN);

	char recv_buf[BUF_LEN];

	// Struct that will hold the IP address of the client that sent the message (we don't have accept() anymore to learn the address)
	struct sockaddr_in fromAddr;
	int fromlen;
	fromlen = sizeof(fromAddr);

	//INITIALIZE YOURSELF
	printf("Enter your username: ");
	std::string line;
	std::getline(std::cin, line);
	line += "\n";
	char* message = (char*)line.c_str();

	//Send message to server
	if (sendto(cliSocket, message, BUF_LEN, 0, ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR)
	{
		printf("sendto() failed %d\n", WSAGetLastError());
		//return 1;
	}


	for (;;)
	{
		printf("Enter message: ");
		std::string line;
		std::getline(std::cin, line);
		line += "\n";
		char* message = (char*)line.c_str();

		//Send message to server
		if (sendto(cliSocket, message, BUF_LEN, 0, ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR)
		{
			printf("sendto() failed %d\n", WSAGetLastError());
			//return 1;
		}

		printf("Message sent...\n");

		/*memset(recv_buf, 0, BUF_LEN);
		if (recvfrom(cliSocket, recv_buf, sizeof(recv_buf), 0, ptr->ai_addr, &fromlen) == SOCKET_ERROR) {
			printf("recvfrom() failed...%d\n", WSAGetLastError());
			return 1;
		}

		printf("Received: %s\n", recv_buf);*/
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