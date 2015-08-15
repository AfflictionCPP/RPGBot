#include "IRCconn.h"
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <time.h>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#define MAXDATASIZE 100

IRCconn::IRCconn(char *_nick, char *_user)
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
	struct addrinfo hints, *servinfo;

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
	if((res = getaddrinfo("irc.quakenet.org", port, &hints, &servinfo)) != 0)
	{
		setup = false;
		cerr << "getaddrinfo : " << gai_strerror(res) << endl;
		WSACleanup();
	}

	// Setup the socket
	if((s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
		cerr << "Client: socket" << endl;

	// Connect
	if(connect(s, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		cerr << "Client: connect" << endl;

	// Servinfo no longer needed
	freeaddrinfo(servinfo);

	// Receive some data
	int numbytes;
	char buf[MAXDATASIZE];

	int count = 0;
	while(1)
	{
		// Declare
		count++;

		switch(count)
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

		// If Ping recieved
		if(charSearch(buf, "PING"))	// Must reply to the ping to avoid connection closure
			sendPong(buf);

		// Break if the connection is closed
		if(numbytes == 0)
		{
			cout << "----------------------CONNECTION CLOSED---------------------------" << endl;
			cout << timeNow() << endl;
			break;
		}
	}
}

/********************************************
*											*
*   Function used to search received data	*
*											*
********************************************/

bool IRCconn::charSearch(char *toSearch, char *searchFor)
{
	int toLen = strlen(toSearch);
	int forLen = strlen(searchFor);

	// Search through each char in toSearch
	for(int i = 0; i < toLen; i++)
	{
		// Search toSearch if the active char is equal to the first search item
		if(searchFor[0] == toSearch[i])
		{
			bool found = true;
			// Search the search field char array
			for(int z = 1; z < forLen; z++)
				if(toSearch[i + z] != searchFor[z])
					found = false;

			if (found) // Return true if found
				return true;
		}
	}
	
	return false;
}

/********************************************
*											*
*	   Function used to send messages		*
*											*
********************************************/

bool IRCconn::sendData(char *msg)
{
	// Send some data
	int len = strlen(msg);
	int bytes_sent = send(s, msg, len, 0);

	if(bytes_sent == 0)
		return false;
	else
		return true;
}

/********************************************
*											*
*	  Function used to respond to PING		*
*											*
********************************************/

void IRCconn::sendPong(char *buf)
{
	// Get the reply address then loop through and find the location of PING
	char *toSearch = "PING ";

	// Search through each char in buf
	for (int i = 0; i < strlen(buf); i++)
	{
		// Search toSearch if the active char is equal to the first search item
		if(buf[i] == toSearch[0])
		{
			bool found = true;
			// Search the search field char array
			for(int z = 1; z < 4; z++)
				if(buf[i + z] != toSearch[z])
					found = false;

			// Return true if found
			if(found)
			{
				// Count the number of chars
				int count = 0;
				for(int z = (i + strlen(toSearch)); z < strlen(buf); z++)
					count++;

				// Create a new char array
				char *returnHost = new char[count + 5];
				returnHost[0] = 'P';
				returnHost[1] = 'O';
				returnHost[2] = 'N';
				returnHost[3] = 'G';
				returnHost[4] = ' ';

				count = 0;

				// Set the hostname data
				for(int z = (i + strlen(toSearch)); z < strlen(buf); z++)
				{
					returnHost[count + 5] = buf[z];
					count++;
				}
				returnHost[count + 5] = '\0';

				// Send the Pong
				if(sendData(returnHost))
					cout << timeNow() << " Ping Pong" << endl;

				return;
			}
		}
	}
}


/********************************************
*											*
*	Function used to get the current time	*
*											*
********************************************/

char *IRCconn::timeNow()
{
	// Returns the current date and time
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	return asctime(timeinfo);
}

/********************************************
*											*
*	  Function used to Handle commands		*
*											*
********************************************/

void IRCconn::msgHandel(char *buf)
{
	/*
	* Add code to respond to commands here
	*/
	if(charSearch(buf, "tits"))	// String of text that you want to trigger on
	{
		sendData("PRIVMSG #SNOXD :(.)(.)\r\n");		// Send a message to the channel specified
	}
}
