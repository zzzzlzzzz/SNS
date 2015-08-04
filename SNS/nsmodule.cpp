#include "nsmodule.h"

using namespace std;

namespace SNS
{
	NSModule::NSModule(const std::string& domfile, unsigned long ttl) :attl(ttl)
	{
		try
		{
			ConfigParser nslist(domfile);
			for (auto nsit = nslist["domains"].begin(); nsit != nslist["domains"].end(); nsit++)
			{
				string ns = convertStr(nsit->second);
				vector<unsigned char> buf(ns.length() + 1);
				memcpy(&buf[0], ns.c_str(), ns.length() + 1);
				nss.emplace(make_pair(nsit->first, buf));
			}
		}
		catch (const exception& e)
		{
			throw logic_error(string("NSModule: ") + e.what());
		}
	}

	bool NSModule::operator ()(DnsResponse& resp, size_t i)
	{
		string domain = resp.getAddress(i);
		auto nsit = nss.find(domain);
		if (nsit != nss.end())
		{
			resp.addRawAnswer(resp.getRaw(i), attl, nsit->second);
			resp.setAnswerCount(resp.getAnswerCount() + 1);
		}
		else
			return false;
		return true;
	}

	string NSModule::convertStr(const std::string& src)
	{
		string s(src);
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
		return s;
	}
}