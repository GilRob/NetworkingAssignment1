///// UDP Server
//Gil Robern - 100651824
//Jessica Le - 100555079
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

int main() {

	//Initialize winsock
	WSADATA wsa;

	int error;
	error = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (error != 0) {
		printf("Failed to initialize %d\n", error);
		return 1;
	}

	//Create a Server socket

	struct addrinfo* ptr = NULL, hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, "8888", &hints, &ptr) != 0) {
		printf("Getaddrinfo failed!! %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	SOCKET server_socket;

	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (server_socket == INVALID_SOCKET) {
		printf("Failed creating a socket %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Bind socket

	if (bind(server_socket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
		printf("Bind failed: %d\n", WSAGetLastError());
		closesocket(server_socket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}

	// Listen on socket

	/*if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Listen failed: %d\n", WSAGetLastError());
		closesocket(server_socket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}

	printf("Waiting for Connections...\n");*/

	// Accept a connection (multiple clients --> threads)

	SOCKET client_socket1;

	client_socket1 = accept(server_socket, NULL, NULL);

	if (client_socket1 == INVALID_SOCKET) {
		printf("Accept() failed %d\n", WSAGetLastError());
		closesocket(server_socket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;

	}

	printf("Client1 connected!!\n");

	SOCKET client_socket2;

	client_socket2 = accept(server_socket, NULL, NULL);

	if (client_socket2 == INVALID_SOCKET) {
		printf("Accept() failed %d\n", WSAGetLastError());
		closesocket(server_socket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;

	}

	printf("Client2 connected!!\n");

	// Receive msg from client
	const unsigned int BUF_LEN = 512;

	char recv_buf[BUF_LEN];

	char send_buf[BUF_LEN];

	// Struct that will hold the IP address of the client that sent the message (we don't have accept() anymore to learn the address)
	struct sockaddr_in fromAddr;
	int fromlen;
	fromlen = sizeof(fromAddr);

	memset(recv_buf, 0, BUF_LEN);
	if (recvfrom(server_socket, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*) & fromAddr, &fromlen) == SOCKET_ERROR) {
		printf("recvfrom() failed...%d\n", WSAGetLastError());
		return 1;
	}

	printf("Welcome to the chat: %s\n", recv_buf);

	for (;;)
	{
		memset(recv_buf, 0, BUF_LEN);
		if (recvfrom(server_socket, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*) & fromAddr, &fromlen) == SOCKET_ERROR) {
			printf("recvfrom() failed...%d\n", WSAGetLastError());
			return 1;
		}

		printf("Received: %s\n", recv_buf);

		char ipbuf[INET_ADDRSTRLEN];
		//		printf("Dest IP address: %s\n", inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));
		//		printf("Source IP address: %s\n", inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));

		/*printf("Enter message: ");
		std::string line;
		std::getline(std::cin, line);
		line += "\n";
		char* message = (char*)line.c_str();

		//Send message to server
		if (sendto(server_socket, message, BUF_LEN, 0, (struct sockaddr*) & fromAddr, ptr->ai_addrlen) == SOCKET_ERROR)
		{
			printf("sendto() failed %d\n", WSAGetLastError());
			return 1;
		}

		printf("Message sent...\n");*/
	}

	closesocket(server_socket);
	freeaddrinfo(ptr);
	WSACleanup();

	return 0;
}