#ifndef SERVER_H
#define SERVER_H

// standart headers
#include <iostream>
#include <stdexcept>
#include <cstring>

// system headers
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

// project headers
#include "config.h"
#include "dnspacket.h"

namespace SNS
{
	class DNServer
	{
		static const int BUFSIZE;
		WSADATA wsaData;
		ConfigParser cfpars;
		DNServer(const std::string& configName);
		virtual ~DNServer();
		DNServer(const DNServer&) = delete;
		DNServer& operator=(const DNServer&) = delete;
	public:
		static DNServer& getInstance(const std::string& configName);
		void mainLoop();
	};
}

#endif // !SERVER_H
