#pragma once
#include <winsock2.h>

class IRCconn
{
public:
	bool setup;

	IRCconn(char* nick, char* usr);
	~IRCconn();
	void start();
private:
	char* port;
	SOCKET s; //the socket descriptor
	char* nick;
	char* user;

	char* timeNow();
	void checkPing(char* buf);
	void msgHandel(char* buf);
	bool sendData(char* msg);
};