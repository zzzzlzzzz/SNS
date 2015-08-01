#ifndef IPFILTER_H
#define IPFILTER_H

// standart header
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <stdexcept>

// project headers
#include "config.h"

namespace SNS
{
	class IpFilter
	{
		bool enable;
		std::set<std::string> ip;
	public:
		IpFilter(ConfigParser& cp);
		~IpFilter() = default;
		bool operator()(const std::string& clip);
	};
}

#endif // !IPFILTER_H
