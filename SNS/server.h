#ifndef SERVER_H
#define SERVER_H

// standart headers
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <memory>
#include <map>
#include <functional>

// system headers
#ifdef _WIN32
	#include <winsock2.h>
	#pragma comment(lib, "ws2_32.lib")
#elif __linux
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
#endif

// project headers
#include "config.h"
#include "ipfilter.h"
#include "dnspacket.h"
#include "amodule.h"
#include "soamodule.h"
#include "cnamemodule.h"
#include "mxmodule.h"
#include "nsmodule.h"

namespace SNS
{
	class DNServer
	{
		static const int BUFSIZE;
        #ifdef _WIN32
            WSADATA wsaData;
        #endif
		ConfigParser cfpars;
		IpFilter filter;
		std::map<RequestType, std::function<bool(DnsResponse&,size_t)>> modules;
		DNServer(const std::string& configName);
		virtual ~DNServer();
		DNServer(const DNServer&) = delete;
		DNServer& operator=(const DNServer&) = delete;
		bool processClient(DnsResponse& resp);
	public:
		static DNServer& getInstance(const std::string& configName);
		void mainLoop();
	};
}

#endif // !SERVER_H
