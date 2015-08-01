#include "amodule.h"

using namespace std;

namespace SNS
{
	AModule::AModule(const string& domfile, unsigned long ttl, unsigned long cachesz) :attl(ttl), cachesize(cachesz)
	{
		try
		{
			ConfigParser cp(domfile);
			for (auto cpit = cp["domains"].begin(); cpit != cp["domains"].end(); cpit++)
			{
				vector<unsigned char> ipbuf;
				strToByte(cpit->second, ipbuf);
				iplist[cpit->first] = ipbuf;
			}
		}
		catch (const exception& e)
		{
			throw logic_error(string("AModule: ") + e.what());
		}
	}

	bool AModule::operator ()(DnsResponse& resp, size_t i)
	{
		string domain = resp.getAddress(i);
		auto it = iplist.find(domain);
		if (it != iplist.end())
		{
			resp.addRawAnswer(resp.getRaw(i), attl, it->second);
			resp.setAnswerCount(resp.getAnswerCount() + 1);
		}
		else
		{
			vector<unsigned char> ipbuf;
			if (!getFromCache(domain, ipbuf))
			{
				hostent* he = gethostbyname(domain.c_str());
				if (he == NULL)
				{
					resp.setReturnCode(NAMENOTEXIST);
					return false;
				}

				IN_ADDR ia;
				ia.s_addr = *reinterpret_cast<unsigned long*>(he->h_addr_list[0]);
				strToByte(string(inet_ntoa(ia)), ipbuf);
				addToCache(domain, ipbuf);
			}
			resp.addRawAddition(resp.getRaw(i), attl, ipbuf);
			resp.setAnswerCount(resp.getAnswerCount() + 1);
		}
		return true;
	}

	void AModule::strToByte(const std::string& src, std::vector<unsigned char>& dst)
	{
		size_t dp = 0, pdp = 0;
		while ((dp = src.find('.', pdp)) != string ::npos)
		{
			dst.push_back(stoi(src.substr(pdp, dp)));
			pdp = ++dp;
		}
		dst.push_back(stoi(src.substr(pdp, src.length())));

		if (dst.size() != 4)
			throw logic_error("ip address not correct");
	}

	void AModule::addToCache(const std::string& domain, const std::vector<unsigned char>& data)
	{
		if (cache.size() >= cachesize)
			cache.pop_back();
		cache.push_front(make_pair(domain, data));
	}

	bool AModule::getFromCache(const std::string& domain, std::vector<unsigned char>& data)
	{
		for (auto it = cache.begin(); it != cache.end(); it++)
		{
			if (it->first == domain)
			{
				data = it->second;
				if (it != cache.begin())
					swap(*it, *(it - 1));
				return true;
			}
		}
		return false;
	}
}