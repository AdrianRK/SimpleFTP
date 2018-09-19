/*
 * =====================================================================================
 *
 *       Filename:  config.hpp
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  08/23/2018 03:30:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SIMPLE_FTP_CONFIG_HEADER
#define SIMPLE_FTP_CONFIG_HEADER
#include <map>
#include <string>

class CConfig
{
public:
	std::string getParameter(const std::string & str);
	static CConfig&	getInstance();

private: 
	CConfig();
	
	CConfig(const CConfig&) = delete;
	const CConfig& operator=(const CConfig&) = delete;
	void loadConfigFile();

	const static std::string configFile;
	const static std::string defaultftpFolderLocation;
	
	std::map <std::string, std::string>	mParameters;
};
#endif // SIMPLE_FTP_CONFIG_HEADER
