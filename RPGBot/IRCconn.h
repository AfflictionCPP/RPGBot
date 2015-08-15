#pragma once
#include <winsock2.h>

class IRCconn
{
public:
	IRCconn(char *nick, char *usr);
	virtual ~IRCconn();

	bool setup;

	void start();
	bool charSearch(char *toSearch, char *searchFor);
private:
	char *port;
	SOCKET s; //the socket descriptor

	char *nick;
	char *user;

	char *timeNow();
	bool sendData(char *msg);
	void sendPong(char *buf);
	void msgHandel(char *buf);
};