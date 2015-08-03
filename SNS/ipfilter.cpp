#include "ipfilter.h"

using namespace std;

namespace SNS
{
        IpFilter::IpFilter(ConfigParser& cp):enable(cp["main"]["ipfiltenabl"] == "1")
	{
                if (enable)
		{
			ifstream ifs(cp["files"]["ipfile"].c_str(), ifstream::in);
			if (!ifs.is_open())
				throw logic_error("IpFilter: filter file not found");

			string buffer;
			while (!ifs.eof())
			{
				buffer.clear();
				getline(ifs, buffer);
				if (!buffer.empty())
					ip.insert(buffer);
			}
			ifs.close();
		}
	}

	bool IpFilter::operator()(const std::string& clip)
	{
		if (!enable || ip.find(clip) != ip.end())
			return true;
		return false;
	}
}
