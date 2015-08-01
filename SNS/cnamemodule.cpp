#include "cnamemodule.h"

using namespace std;

namespace SNS
{
	CNAMEModule::CNAMEModule(const string& domfile, unsigned long ttl) :attl(ttl)
	{
		try
		{
			ConfigParser cnp(domfile);
			for (auto cnit = cnp["cnames"].begin(); cnit != cnp["cnames"].end(); cnit++)
			{
				string buf = convertStr(cnit->second);
				vector<unsigned char> vbuf(buf.length() + 1);
				memcpy(&vbuf[0], buf.c_str(), buf.length() + 1);
				alliases.emplace(make_pair(cnit->first, vbuf));
			}
		}
		catch (const exception& e)
		{
			throw logic_error(string("CNAMEModule: ") + e.what());
		}
	}

	string CNAMEModule::convertStr(const string& src) const
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

	bool CNAMEModule::operator ()(DnsResponse& resp, size_t i)
	{
		string domain = resp.getAddress(i);
		auto alit = alliases.find(domain);
		if (alit != alliases.end())
		{
			resp.addRawAnswer(resp.getRaw(i), attl, alit->second);
			resp.setAnswerCount(resp.getAnswerCount() + 1);
		}
		else
			return false;
		return true;
	}
}