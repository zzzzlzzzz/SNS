#ifndef NSMODULE_H
#define NSMODULE_H

// standart header
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>
#include <cstring>

// project header
#include "dnspacket.h"
#include "config.h"

namespace SNS
{
	class NSModule
	{
		unsigned long attl;
		std::map<std::string, std::vector<unsigned char>> nss;
		std::string convertStr(const std::string& src);
	public:
		NSModule(const std::string& domfile, unsigned long ttl);
		bool operator()(DnsResponse& resp, size_t i);
	};
}

#endif // !NSMODULE_H
