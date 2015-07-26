#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <stdexcept>
#include <cstring>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

namespace SNS
{
	class DNServer
	{
		static const int BUFSIZE;
		WSADATA wsaData;
		DNServer();
		virtual ~DNServer();
		DNServer(const DNServer&) = delete;
		DNServer& operator=(const DNServer&) = delete;
	public:
		static DNServer& getInstance();
		void mainLoop();
	};
}

#endif // !SERVER_H
