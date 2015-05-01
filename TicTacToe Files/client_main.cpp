// client_main.cpp
//   This function serves as the "main" function for the client-side
#include "Nim.h"
#include <iostream>
#include <string>
#include <winsock2.h>

void client_main(int argc, char *argv[], std::string client_name)
{
	std::string host;
	std::string port;
	ServerStruct server[MAX_HOST_SIZE];
	bool denied_connection = false;
	bool isClient = true;

	SOCKET s = connectsock("", "", "udp");		// Create a socket  (Don't need to designate a host or port for UDP

	// Find all Nim servers
	std::cout << std::endl << "Looking for Nim servers ... " << std::endl;
	int numServers;
	getServers(s, "10.1.255.255", UDPPORT_NIM, server, numServers);

	//192.168.1.255 for home

	if (numServers == 0) {
		std::cout << std::endl << "Sorry.  No Nim servers were found.  Try again later." << std::endl << std::endl;
	}
	else {
		// Display the list of servers found
		std::cout << std::endl << "Found Nim server";
		if (numServers == 1) {
			std::cout << ":" << "  " << server[0].name << std::endl;
		}
		else {
			std::cout << "s:" << std::endl;
			for (int i = 0; i < numServers; i++) {
				std::cout << "  " << i + 1 << " - " << server[i].name << std::endl;
			}
			std::cout << std::endl << "  " << numServers + 1 << " - QUIT" << std::endl;
		}
		std::cout << std::endl;

		// Allow user to select someone to challenge
		do
		{
			denied_connection = false;

			int answer = 0;
			std::string answer_str;
			if (numServers == 1) {
				std::cout << "Do you want to challenge " << server[0].name << "? ";
				std::getline(std::cin, answer_str);
				if (answer_str[0] == 'y' || answer_str[0] == 'Y') answer = 1;
			}
			else if (numServers > 1) {
				std::cout << "Who would you like to challenge (1-" << numServers + 1 << ")? ";
				std::getline(std::cin, answer_str);
				answer = atoi(answer_str.c_str());
				if (answer > numServers) answer = 0;
			}

			if (answer >= 1 && answer <= numServers)
			{
				// Extract the opponent's info from the server[] array
				std::string serverName;
				serverName = server[answer - 1].name;		// Adjust for 0-based array
				host = server[answer - 1].host;
				port = server[answer - 1].port;

				// Append playerName to the Nim_CHALLENGE string & send a challenge to host:port
				char buf[MAX_SEND_BUF];
				strcpy_s(buf, Nim_CHALLENGE);
				strcat_s(buf, client_name.c_str());
				int len = UDP_send(s, buf, strlen(buf) + 1, (char*)host.c_str(), (char*)port.c_str());

				int waitTime = wait(s, 20, 0);
				cout << waitTime;

				if (waitTime == 0)
				{
					denied_connection = true;
				}
				else
				{
					int recv = UDP_recv(s, buf, strlen(buf) + 1, (char*)host.c_str(), (char*)port.c_str());
					std::cout << "Receiving: " << buf << std::endl;	// DEBUG

					if (_stricmp(buf, "YES\0") == 0)
					{
						int len = UDP_send(s, "GREAT!\0", 9, (char*)host.c_str(), (char*)port.c_str());
						std::cout << "Sending: GREAT!" << std::endl;	// DEBUG

						closesocket(s);
						// Play the game.  You are the first player

						SOCKET tcp = connectsock((char*)host.c_str(), TCPPORT_NIM, "tcp");

						play_Nim(tcp, isClient, (char*)client_name.c_str());					
					}

				}
				if (_stricmp(buf, "NO\0") == 0 || denied_connection)
				{

					if (numServers == 1)
					{
						denied_connection = true;
					}
					else 
					{
						char answer;

						std::cout << "Do you want to quit? (q for quit)";
						std::cin >> answer;
						if (answer != 'q')
						{
							denied_connection = true;
						}
					}

				}
			}
		} while (!denied_connection);

		closesocket(s);
	}
}