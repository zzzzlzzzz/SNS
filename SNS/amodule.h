#ifndef AMODULE_H
#define AMODULE_H

// standart headers
#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <stdexcept>

// system headers
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

// project headers
#include "dnspacket.h"
#include "config.h"

namespace SNS
{
	class AModule
	{
		unsigned long attl;
		unsigned long cachesize;
		std::deque<std::pair<std::string, std::vector<unsigned char>>> cache;
		bool getFromCache(const std::string& domain, std::vector<unsigned char>& data);
		void addToCache(const std::string& domain, const std::vector<unsigned char>& data);
		std::map<std::string, std::vector<unsigned char>> iplist;
		void strToByte(const std::string& src, std::vector<unsigned char>& dst);
	public:
		AModule(const std::string& domfile, unsigned long ttl, unsigned long cachesz);
		~AModule() = default;
		bool operator()(DnsResponse& resp, size_t i);
	};
}

#endif // !AMODULE_H
