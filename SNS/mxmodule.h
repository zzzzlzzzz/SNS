#ifndef MXMODULE_H
#define MXMODULE_H

// standart header
#include <iostream>
#include <stdexcept>
#include <map>
#include <vector>

// system header
#ifdef _WIN32
	#include <WinSock2.h>
#elif __linux
	#include <arpa/inet.h>
	#include <netdb.h>
#endif

// project header
#include "config.h"
#include "dnspacket.h"

namespace SNS
{
	class MXModule
	{
		unsigned long attl;
		std::map<std::string, std::vector<unsigned char>> mxs;
		std::string convertStr(const std::string& src) const;
	public:
		MXModule(const std::string& domfile, unsigned long ttl);
		bool operator()(DnsResponse& resp, size_t i);
	};
}

#endif // !MXMODULE_H
