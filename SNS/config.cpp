#include "config.h"

using namespace std;

namespace SNS
{
	ConfigParser::ConfigParser(const std::string& name)
	{
		ifstream ifs(name.c_str(), ifstream::in);
		if (!ifs.is_open())
			throw runtime_error("ConfigParser: config not exists");

		string curcat = "";
		while (!ifs.eof())
		{
			string cline("");
			getline(ifs, cline);
			if (cline[0] == '#' || cline.empty())
				continue;
			else if (cline[0] == '[')
				curcat = cline.substr(1, cline.length() - 2);
			else if (!curcat.empty())
			{
				size_t ep = cline.find("=");
				if (ep != string::npos)
					config[curcat][cline.substr(0, ep)] = cline.substr(ep + 1, cline.length());
			}
		}
		ifs.close();
	}

	map<string, string>& ConfigParser::operator[](const string& idx)
	{
		return config[idx];
	}

	map<string, map<string, string>>::iterator ConfigParser::begin()
	{
		return config.begin();
	}

	map<string, map<string, string>>::iterator ConfigParser::end()
	{
		return config.end();
	}
}