#ifndef SOAMODULE_H
#define SOAMODULE_H

// standart header
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>
#include <cstring>

// system headers
#ifdef _WIN32
	#include <WinSock2.h>
#endif

// project headers
#include "config.h"
#include "dnspacket.h"

namespace SNS
{
	class SOAModule
	{
		unsigned long attl;
		std::map<std::string, std::vector<unsigned char>> domains;
		void convertStr(std::string& s) const;
	public:
		SOAModule(const std::string& domfile, unsigned long ttl);
		bool operator()(DnsResponse& resp, size_t i);
	};
}

#endif // !SOAMODULE_H
