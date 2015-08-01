#ifndef CNAMEMODULE_H
#define CNAMEMODULE_H

// standart header
#include <iostream>
#include <map>
#include <vector>
#include <cstring>

// project headers
#include "config.h"
#include "dnspacket.h"

namespace SNS
{
	class CNAMEModule
	{
		unsigned long attl;
		std::map<std::string, std::vector<unsigned char>> alliases;
		std::string convertStr(const std::string& src) const;
	public:
		CNAMEModule(const std::string& domfile, unsigned long ttl);
		bool operator()(DnsResponse& resp, size_t i);
	};
}

#endif // !CNAMEMODULE_H
