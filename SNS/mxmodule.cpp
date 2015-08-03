#include "mxmodule.h"

using namespace std;

namespace SNS
{
	MXModule::MXModule(const string& domfile, unsigned long ttl) :attl(ttl)
	{
		try
		{
			ConfigParser cfmx(domfile);
			for (auto mxit = cfmx.begin(); mxit != cfmx.end(); mxit++)
			{
				unsigned short dmxp = htons(stoi(mxit->second["preference"]));
				string dmx = convertStr(mxit->second["exchange"]);
				vector<unsigned char> buffer(sizeof(dmxp)+dmx.length() + 1);
				memcpy(&buffer[0], &dmxp, sizeof(dmxp));
				memcpy(&buffer[sizeof(dmxp)], dmx.c_str(), dmx.length() + 1);
				mxs.emplace(make_pair(mxit->first, buffer));
			}
		}
		catch (const exception& e)
		{
			throw logic_error(string("MXModule: ") + e.what());
		}
	}

	bool MXModule::operator ()(DnsResponse& resp, size_t i)
	{
		string domain = resp.getAddress(i);
		auto mxit = mxs.find(domain);
		if (mxit != mxs.end())
		{
			resp.addRawAnswer(resp.getRaw(i), attl, mxit->second);
			resp.setAnswerCount(resp.getAnswerCount() + 1);
		}
		else
			return false;
		return true;
	}

	string MXModule::convertStr(const string& src) const
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