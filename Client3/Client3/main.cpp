#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include <sstream>

#pragma comment(lib, "ws2_32.lib") //He did a capital 'W'

SOCKET cliSocket;
bool isRunning = true;
char buf[4096];
std::string userInput;
std::string ipAddress;
std::string user;

///FUNCTION TO RECIEVE MESSAGES///
void MsgReceive()
{
	while (isRunning == true)
	{
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(cliSocket, buf, 4096, 0);
		if (bytesReceived > 0)
		{
			// Echo response to console - this is what collects message from the server
			std::cout << std::string(buf, 0, bytesReceived) << std::endl;
		}
	}
}
///FUNCTION TO SEND MESSAGES///
void MsgSend()
{
	while (isRunning == true)
	{
		std::cout << ">";
		std::getline(std::cin, userInput);
		if (userInput.size() > 0)		// Make sure the user has typed in something
		{
			std::ostringstream ss;
			ss << user << ": " << userInput.c_str() << "\r\n";
			std::string strOut = ss.str();
			// Send the text
			int sendResult = send(cliSocket, strOut.c_str(), strOut.size() + 1, 0);
		}
	}
}
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

	std::cout << "Enter the host IP address> ";
	std::getline(std::cin, ipAddress);
	const char* theAddy = ipAddress.c_str();

	//Instead of "localhost" he did "127.0.0.1" could apparently use either (they are the same??)
	if (getaddrinfo(theAddy, "5000", &hints, &ptr) != 0)
	{
		printf("Getaddrinfo failed! %d\n", WSAGetLastError());
		WSACleanup(); //Cleanup the resources we were using
		return 1;
	}

	cliSocket = socket(AF_INET, SOCK_STREAM, 0);

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

	std::cout << "Enter your username> ";
	std::getline(std::cin, user);

	//Create a buffer

	const unsigned int BUF_LEN = 512;

	//char recv_buf[BUF_LEN];
	//memset(recv_buf, 0, BUF_LEN);

	//Change to non-blocking mode
	u_long mode = 1; // 0 is for blocking, 1 is for non blocking
	ioctlsocket(cliSocket, FIONBIO, &mode);

	int sError = -1;
	int bytes_received = -1;

	//bytes_received = recv(cliSocket, recv_buf, BUF_LEN, 0);
	//sError = WSAGetLastError();
	//if (recv(cliSocket, recv_buf, BUF_LEN, 0) > 0)
	//	printf("Received from server: %s\n", recv_buf);
	//else printf("recv() error: %d\n", WSAGetLastError());

	std::thread t1(MsgReceive);
	std::thread t2(MsgSend);


	while (isRunning == true)
	{
		t1.join();
		t2.join();
	}
	closesocket(cliSocket);
	WSACleanup();

	//if (shutdown(cliSocket, SD_BOTH) == SOCKET_ERROR)
	//{
	//	printf("Shutdown failed! %d\n", WSAGetLastError());
	//	closesocket(cliSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//
	//closesocket(cliSocket);
	//freeaddrinfo(ptr);
	//WSACleanup();
	//
	//return 0;
}