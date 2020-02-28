#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

#pragma comment(lib, "ws2_32.lib") //He did a capital 'W'
char buf[4096];
SOCKET sock;
fd_set master;
SOCKET serverSocket;
SOCKET outSock ;

void printOnline()
{
	//for the number of sockets in master (where its stored)
	for (int i = 0; i < master.fd_count; i++)
	{
		outSock = master.fd_array[i];

		std::ostringstream oss;
		oss << "SOCKET #" << outSock << ": IS ONLINE TEST" << buf << "\r\n";
		//std::string strOut = oss.str();

		std::cout << outSock<< std::endl;
		//send(outSock, strOut.c_str(), strOut.size() + 1, 0);
		//if (outSock != serverSocket && outSock != sock)
		//{
		//	std::ostringstream ss;
		//	ss << "SOCKET #" << sock << ": IS ONLINE" << buf << "\r\n";
		//	std::string strOut = ss.str();
		//
		//	send(outSock, strOut.c_str(), strOut.size() + 1, 0);
		//}
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

	//if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
	//{
	//	printf("Listen failed: %d\n", WSAGetLastError());
	//	closesocket(serverSocket);
	//	freeaddrinfo(ptr);
	//	WSACleanup();
	//	return 1;
	//}
	listen(serverSocket, SOMAXCONN);

	printf("Waiting for connections...\n");

	//Create the master file descriptor set and zero it
	FD_ZERO(&master);

	FD_SET(serverSocket, &master);

	bool running = true;

	while (running)
	{
		fd_set copy = master;

		//Who talking?
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			sock = copy.fd_array[i];

			if (sock == serverSocket)
			{
				//Accept new connection
				SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

				//Add new connection
				FD_SET(clientSocket, &master);

				//Send welcome message
				std::string welcomeMsg = "Welcome asshole!\r\n";
				send(clientSocket, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

				for (int i = 0; i < master.fd_count; i++)
				{
					outSock = master.fd_array[i];
					if (outSock != serverSocket && outSock != sock)
					{
						std::ostringstream ss;
						ss << "SOCKET #" << clientSocket << ": IS ONLINE" << buf << "\r\n";
						std::string strOut = ss.str();

						send(outSock, strOut.c_str(), strOut.size() + 1, 0);
					}
				}
			}
			else //inbound message
			{
				ZeroMemory(buf, 4096);

				//Receive message
				int bytesIn = recv(sock, buf, 4096, 0);

				if (bytesIn <= 0)
				{
					//Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else if (buf[0]== 'd')
				{
					printOnline();
				}
				else if (buf[0] == 's')
				{
					std::cout << serverSocket << std::endl;
				}
				else if (buf[0] == 'o')
				{
					std::cout << outSock << std::endl;
				}
				else if (buf[0] == 'o')
				{
					std::cout << sock << std::endl;
				}
				else
				{
					//Checl to see if it is a comman. \quit kills the server
					if (buf[0] == '\\')
					{
						//Is the command quit?
						std::string cmd = std::string(buf, bytesIn);
						if (cmd == "\\quit")
						{
							running = false;
							break;
						}

						//unknown command
						continue;
					}

					//Send message to other clients, and definiately NOT the listening socket

					for (int i = 0; i < master.fd_count; i++)
					{
						outSock = master.fd_array[i];
						if (outSock != serverSocket && outSock != sock)
						{
							std::ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buf << "\r\n";
							std::string strOut = ss.str();

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
			}
		}
	}

	//Remove the listening socket from the master file descriptor set and close it to prevent anyone else trying to connect
	FD_CLR(serverSocket, &master);
	closesocket(serverSocket);

	//Message to let users know what is happening
	std::string msg = "Server is closing, pack your bags and GTFO!.\r\n";

	while (master.fd_count > 0)
	{
		//Get the socket number
		SOCKET sock = master.fd_array[0];

		//Send peace out message
		send(sock, msg.c_str(), msg.size() + 1, 0);

		//Remove it from master file list and close socket
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	//Cleanup winsock
	WSACleanup();

	//Accept a connection (multiple clients --> threads)

	/*SOCKET clientSocket;

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

	if (shutdown(clientSocket, SD_BOTH) == SOCKET_ERROR)
	{
		printf("Shutdown failed! %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	closesocket(serverSocket);
	freeaddrinfo(ptr);
	WSACleanup();*/

	return 0;
}