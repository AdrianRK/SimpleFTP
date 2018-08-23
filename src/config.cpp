/*
 * =====================================================================================
 *
 *       Filename:  config.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/23/2018 15:21:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "../inc/config.hpp"
#include "../inc/logging.hpp"
#include <fstream>

const std::string CConfig::configFile = "configFile.ini";
const std::string CConfig::defaultftpFolderLocation = ".";

std::string CConfig::getParameter(const std::string &st)
{
	return mParameters[st];
}

CConfig& CConfig::getInstance()
{
	static CConfig cfg;
	return cfg;
}

std::string rtrim(const std::string &str, char c)
{
	for(std::string::riterator it = str.rbegin(); it != str.rend(); ++it)
	{
		if (it != c)
	}
}

void CConfig::loadConfigFile()
{
	std::ifstream fd(CConfig::configFile);

	while(!fd.eof())
	{
		char s[256] = {0,};
		std::string key, value, line;	
		fd.getline(s, 255);
		line = s;
		if (line.size())
		{
			size_t pos1 = line.find("=");
			printLog(line);
			if (pos1 == std::string::npos)
			{
				continue;
			}
			size_t pos2 = line.find("#", pos1+1);
			if (pos2 == std::string::npos)
			{
				pos2 = line.size();
			}
			printLog(pos1);
			key			= ((line.substr(0, pos1)).ltrim()).rtrim();
			printLog(key);
			value		= ((line.substr(pos1 + 1, line.size())).ltrim()).rtrim();
			printLog(value);
			mParameters.insert(std::pair<std::string, std::string>(key,value));
		}
	}

	fd.close();
}

CConfig::CConfig()
{
	loadConfigFile();
}
