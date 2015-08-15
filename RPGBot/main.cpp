#include "IRCconn.h"

int main()
{
	// NICK requires a single name for your bot to use
	// USER requires three names for your bot to use
	// USER: requires a single name for your bot to use
	IRCconn conn = IRCconn("NICK ExampleNick\r\n", "USER ExampleUser ExampleUserTwo ExampleUserThree :ExampleUser\r\n");
	conn.start();

	return 0;
}
