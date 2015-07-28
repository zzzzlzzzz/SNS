#include <iostream>
#include "server.h"

/*
*	SNS - Simple Name Server
*/

int main(int argc, char* argv[])
{
	using namespace std;
	using namespace SNS;

	if (argc < 2)
	{
		cout << "Config file not selected. Run: " << argv[0] << " <config file>" << endl;
		return 0;
	}

	try
	{
		DNServer& server = DNServer::getInstance(string(argv[1]));
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