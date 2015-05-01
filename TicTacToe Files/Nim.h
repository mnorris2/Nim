// Nim.h
//
// This header file establishes some constants that should be used
// in the Nim project for COMP 311
#include <winsock2.h>
#include <string>
using namespace std;
#pragma comment (lib,"ws2_32.lib")

static char BROADCAST_ADDR[] = "10.1.255.255";
static char UDPPORT_NIM[] = "29333";
static char TCPPORT_NIM[] = "29334";

#define Nim_QUERY  "Name?"
#define Nim_NAME	 "Name="
#define Nim_CHALLENGE "Play? Name="

static char board[80];

struct ServerStruct {
	std::string name;
	std::string host;
	std::string port;
};

const int MAX_SEND_BUF = 2048;
const int MAX_RECV_BUF = 2048;
const int MAX_HOSTNAME = 1024;
const int MAX_HOST_SIZE = 16;
const int MAX_SERVICE_SIZE = 80;
const int MAX_LINE = 1024;
const int MAX_NAME = 1024;
const int MAX_HOST = 100;
const int MAXROCKS = 20;
const int MAXPILES = 9;

void   chomp(char*);
void   server_main(int, char*[], std::string);
void   client_main(int, char*[], std::string);
void   getServers(SOCKET, char *, char *, ServerStruct[], int &);
void   play_Nim(SOCKET, bool, char*);
int    UDP_recv(SOCKET, char*, int, char*, char*);
int    UDP_send(SOCKET, char*, int, char*, char*);
int    wait(SOCKET, int, int);
SOCKET passivesock(char*, char*);
SOCKET connectsock(char*, char*, char*);

