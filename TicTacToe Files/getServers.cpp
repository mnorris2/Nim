#include "Nim.h"
#include <winsock2.h>
#include <iostream>
#include <string>

// getServers.cpp
//  Given a socket, a broadcast address and a port number, this function returns the number
//  of servers as well as an array of structs.  Each struct contains the name, IP_Address and 
//  port number of a remote server.

//	IN parameters:
//	SOCKET s				= Allocate UDP socket handle that can be used for communcations
//	char *broadcastAddress	= The broadcast address that should be used for current LAN
//	char *broadcastPort		= The port that should be used for the broadcast

//	OUT parameters:
//	ServerStruct server[]	= An array that contains the structs.  The members of each struct
//							  contain the name, IP Address and port number of remote server.
//	numServers				= Number of elements in the server[] array

//	Function return value	= Number of remote servers (size of server[] array)

void getServers(SOCKET s, char *broadcastAddress, char *broadcastPort, ServerStruct server[], int &numServers)
{
	std::string host;
	std::string port;

	numServers = 0;

	// Send Nim_QUERY to broadcastAddress using broadcastPort
//	int len = UDP_send(s, Nim_QUERY, strlen(Nim_QUERY)+1, broadcastAddress, broadcastPort);
	char buf[MAX_SEND_BUF];
	strcpy_s (buf, Nim_QUERY);
	int len = UDP_send(s, buf, strlen(buf)+1, broadcastAddress, broadcastPort);

	// Receive incoming UDP datagrams (with a maximum of 2 second wait before each recv() function call)
	// As you read datagrams, if they start with the prefix: Nim_NAME, parse out the server's name
	// and add the name, host address and port number to the server[] array.  Don't forget to increment numServers.
	int status = wait(s,2,0);
	if (status > 0) {
		int len = UDP_recv(s, buf, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
		while (status > 0 && len > 0) {
			char *startOfName = strstr(buf,Nim_NAME);
			if (startOfName != NULL) {
				server[numServers].name = startOfName+strlen(Nim_NAME);
				server[numServers].host = host.c_str() ;
				server[numServers].port = port.c_str();
				numServers++;
			}
			status = wait(s,2,0);
			if (status > 0)
				len = UDP_recv(s, buf, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
		}
	}
}
