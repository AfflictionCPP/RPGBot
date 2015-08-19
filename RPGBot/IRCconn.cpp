#include "IRCconn.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <time.h>
#include <iostream>
#include <array>
using namespace std;

#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")
#define MAXDATASIZE 100

IRCconn::IRCconn(char* _nick, char* _user)
{
	nick = _nick;
	user = _user;
}

IRCconn::~IRCconn()
{
	closesocket(s);
}

/********************************************
*											*
*    Function used to setup and connect		*
*											*
********************************************/

void IRCconn::start()
{
	struct addrinfo hints;
	struct addrinfo* servinfo;

	// Setup ran with no errors
	setup = true;

	port = "6667";

	// Ensure that servinfo is clear
	memset(&hints, 0, sizeof hints); // Set the struct to empty

									 // Setup hints
	hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

									 //Start WSA to be able to make DNS lookup
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Setup the structs and display any errors
	int res;
	if ((res = getaddrinfo("irc.quakenet.org", port, &hints, &servinfo)) != 0)
	{
		setup = false;
		cerr << "getaddrinfo : " << gai_strerror(res) << endl;
		WSACleanup();
	}

	// Setup the socket
	if ((s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
		cerr << "Client: socket" << endl;

	// Connect
	if (connect(s, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		cerr << "Client: connect" << endl;

	// Servinfo no longer needed
	freeaddrinfo(servinfo);

	// Receive some data
	int numbytes;
	char buf[MAXDATASIZE];

	int count = 0;
	while (1)
	{
		// Declare
		count++;

		switch (count)
		{
		case 3: // Send data to the server after 3 receives (IRC protocol)
			sendData(nick);
			sendData(user);
			break;
		case 5: // Join a channel
			sendData("JOIN #SNOXD\r\n");
		default:
			break;
		}

		// Receive and print data
		numbytes = recv(s, buf, MAXDATASIZE - 1, 0);
		buf[numbytes] = '\0';
		cout << buf; // Buf is the recieved data

					 // Pass buf to the message handler
		msgHandel(buf);

		// Check if Ping recieved
		checkPing(buf);

		// Break if the connection is closed
		if (numbytes == 0)
		{
			cout << "----------------------CONNECTION CLOSED---------------------------" << endl;
			cout << timeNow() << endl;
			break;
		}
	}
}

/********************************************
*											*
*	  Function used to Handle commands		*
*											*
********************************************/

void IRCconn::msgHandel(char* buf)
{
	char _nick[MAXDATASIZE];
	char _user[MAXDATASIZE];
	char _host[MAXDATASIZE];
	char _command[MAXDATASIZE];
	char _channel[MAXDATASIZE];
	char _message[MAXDATASIZE];

	if (sscanf(buf, ":%[^!]!%[^@]@%s %s %s :%[^\r][\r]", _nick, _user, _host, _command, _channel, _message) == 6)
	{
		
		
		char* messageFound = NULL;
		char* playerFound = NULL;

		bool playerExists = false;
		std::string playerName[5];
		std::string playerHost[5];

		playerHost[0] = " ARRAY0 ";
		playerHost[1] = " ARRAY1 ";
		playerHost[3] = " ARRAY3 ";

		



		if (messageFound = strstr(_message, "test"))
			{
				//Check if host has already registered , if true , skip to else if
				//If false , continue within nested ifs
				int i = 0;
				for (;i < 50; i++)
				{
					
					if (_host != 0)
					{
						if (playerHost[i].c_str() != _host)
							playerHost[i] = _host;
						printf(playerHost[0].c_str());
						printf(playerHost[1].c_str());
						printf(playerHost[2].c_str());
						printf(playerHost[3].c_str());
						break;
					
					}
				}
			}
			
			
		
	

	//sendData("PRIVMSG #SNOXD :Oh hi\r\n");

	}
		
	
}

/********************************************
*											*
*	  Function used to respond to PING		*
*											*
********************************************/

void IRCconn::checkPing(char* buf)
{
	if (strncmp("PING :", buf, 6) == 0)
	{
		// Turn PING into PONG
		buf[1] = 'O';

		// Send the Pong
		if (sendData(buf))
			cout << timeNow() << " Ping Pong" << endl;
	}
}

/********************************************
*											*
*	   Function used to send messages		*
*											*
********************************************/

bool IRCconn::sendData(char* msg)
{
	// Send some data
	int len = strlen(msg);
	int bytes_sent = send(s, msg, len, 0);

	if (bytes_sent == 0)
		return false;
	else
		return true;
}

/********************************************
*											*
*	Function used to get the current time	*
*											*
********************************************/

char* IRCconn::timeNow()
{
	// Returns the current date and time
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	return asctime(timeinfo);
}