#include "soamodule.h"

using namespace std;

namespace SNS
{
	SOAModule::SOAModule(const std::string& domfile, unsigned long ttl) :attl(ttl)
	{
		#pragma pack(push,1)
		struct SOAData
		{
			unsigned long serial;
			unsigned long refresh;
			unsigned long retry;
			unsigned long expire;
			unsigned long defaulttls;
		};
		#pragma pack(pop)
		try
		{
			ConfigParser cfp(domfile);
			for (auto cfit = cfp.begin(); cfit != cfp.end(); cfit++)
			{
				string mname = cfit->second["mname"];
				convertStr(mname);
				string rname = cfit->second["rname"];
				convertStr(rname);
				SOAData data;
				data.serial = htonl(stol(cfit->second["serial"]));
				data.refresh = htonl(stol(cfit->second["refresh"]));
				data.retry = htonl(stol(cfit->second["retry"]));
				data.expire = htonl(stol(cfit->second["expire"]));
				data.defaulttls = htonl(stol(cfit->second["defaulttls"]));
				
				vector<unsigned char> buf(mname.length() + 1 + rname.length() + 1 + sizeof(SOAData));
				memcpy(&buf[0], mname.c_str(), mname.length() + 1);
				memcpy(&buf[mname.length() + 1], rname.c_str(), rname.length() + 1);
				memcpy(&buf[mname.length() + 1 + rname.length() + 1], &data, sizeof(SOAData));
				domains.emplace(make_pair(cfit->first, buf));
			}
		}
		catch (const exception& e)
		{
			throw logic_error(string("SOAModule: ") + e.what());
		}
	}

	void SOAModule::convertStr(std::string& s) const
	{
		unsigned char cnt = 0;
		for (auto sit = s.rbegin(); sit != s.rend(); sit++)
		{
			if (*sit == '.')
			{
				*sit = cnt;
				cnt = 0;
			}
			else
				++cnt;
		}
		s = string(1, cnt) + s;
	}

	bool SOAModule::operator()(DnsResponse& resp, size_t i)
	{
		string domain = resp.getAddress(i);
		auto itd = domains.find(domain);
		if (itd != domains.end())
		{
			resp.addRawAnswer(resp.getRaw(i), attl, itd->second);
			resp.setAnswerCount(resp.getAnswerCount() + 1);
		}
		else
			return false;

		return true;
	}
}