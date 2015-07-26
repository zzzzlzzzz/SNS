#include <iostream>
#include "server.h"

/*
*	SNS - Simple Name Server
*/

int main(int argc, char* argv[])
{
	using namespace std;
	using namespace SNS;
	try
	{
		DNServer& server = DNServer::getInstance();
		server.mainLoop();
	}
	catch (const exception& e)
	{
		cerr << "Error: " << e.what() << endl;
	}
	catch (...)
	{
		cerr << "Undetected critical error" << endl;
	}
	return 0;
}