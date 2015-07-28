#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <stdexcept>
#include <string>
#include <map>

namespace SNS
{
	class ConfigParser
	{
		std::map<std::string, std::map<std::string, std::string>> config;
	public:
		ConfigParser(const std::string& name);
		std::map<std::string, std::string>& operator[](const std::string& idx);
	};
}

#endif // !CONFIG_H
