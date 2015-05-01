// server_main.cpp
//   This function serves as the "main" function for the server-side
#include "Nim.h"
#include <iostream>
#include <string>
#include <winsock2.h>

void server_main(int argc, char *argv[], std::string server_name)
{
	SOCKET s;
	char buf[MAX_RECV_BUF];
	std::string host;
	std::string port;
	char response_str[MAX_SEND_BUF];
	bool isClient = false;
	
	s = passivesock(UDPPORT_NIM,"udp");

	std::cout << std::endl << "Waiting for a challenge..." << std::endl;
	int len = UDP_recv(s, buf, MAX_RECV_BUF, (char*)host.c_str() , (char*)port.c_str());
	std::cout << "Received: " << buf << std::endl;  // For DEBUG purposes

	bool finished = false;
	while (!finished) {
		if ( strcmp(buf, Nim_QUERY) == 0) {
			// Respond to name query
			strcpy_s(response_str,Nim_NAME);
			strcat_s(response_str, (char*)server_name.c_str());
			UDP_send(s, response_str, strlen(response_str)+1, (char*)host.c_str(), (char*)port.c_str());
			std::cout << "Sending: " << response_str << std::endl;	// DEBUG

		} else if ( strncmp(buf, Nim_CHALLENGE, strlen(Nim_CHALLENGE)) == 0) {
			// Received a challenge  
			char *startOfName = strstr(buf,Nim_CHALLENGE);
			if (startOfName != NULL) {
				std::cout << std::endl << "You have been challenged by " << startOfName + strlen(Nim_CHALLENGE) << std::endl;

				char answer;

				cout << "Do you want to play " << startOfName + strlen(Nim_CHALLENGE) << "? (y for yes, n for no)" << std::endl;
				cin >> answer;

				if (answer == 'y')
				{
					SOCKET tcp = passivesock(TCPPORT_NIM, "tcp");

					UDP_send(s, "YES\0", 5, (char*)host.c_str(), (char*)port.c_str());
					std::cout << "Sending: YES" << std::endl;	// DEBUG

					int time = wait(s, 5, 0);

					if (time == 0)
					{
						closesocket(tcp);
					}
					else
					{
						UDP_recv(s, buf, strlen(buf) + 1, (char*)host.c_str(), (char*)port.c_str());
						std::cout << "Receiving: " << buf << std::endl;	// DEBUG
					}
					if (_stricmp(buf, "GREAT!\0") == 0)
					{
						closesocket(s);
						// Play the game.  You are the player to go second
						play_Nim(tcp, isClient, (char*)server_name.c_str());
						finished = true;
					}

				}
				else if (answer == 'n')
				{
					UDP_send(s, "NO\0", 5, (char*)host.c_str(), (char*)port.c_str());
					std::cout << "Sending: NO" << std::endl;	// DEBUG
				}
			}
		}

		if (!finished) {
			char previous_buf[MAX_RECV_BUF];		strcpy_s(previous_buf,buf);
			std::string previous_host;				previous_host = host;
			std::string previous_port;				previous_port = port;

			// Check for duplicate datagrams (can happen if broadcast enters from multiple ethernet connections)
			bool newDatagram = false;
			int status = wait(s,1,0);	// We'll wait a second to see if we receive another datagram
			while (!newDatagram && status > 0) {
				len = UDP_recv(s, buf, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
				std::cout << "Received: " << buf << std::endl;	// DEBUG
				if (strcmp(buf,previous_buf)==0 &&		// If this datagram is identical to previous one, ignore it.
					host == previous_host && 
					port == previous_port) {
						status = wait(s,1,0);			// Wait another second (still more copies?)
				} else {
					newDatagram = true;		// if not identical to previous one, keep it!
				}
			}

			// If we waited one (or more seconds) and received no new datagrams, wait for one now.
			if (!newDatagram ) {
				len = UDP_recv(s, buf, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
				std::cout << "Received: " << buf << std::endl;	// DEBUG
			}
		}
	}
	closesocket(s);
}