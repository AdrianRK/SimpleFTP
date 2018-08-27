/*
 * =====================================================================================
 *
 *       Filename:  ui.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/27/2018 09:40:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SIMPLE_FTP_USER_INTERFACE_HEADER
#define SIMPLE_FTP_USER_INTERFACE_HEADER

#include <map>
#include <string>

class CCommands
{
public:
	virtual ~CCommands() {}
	virtual void operator ()() = 0;
};

class CUserInterface
{
public:
	typedef std::map<char, std::pair<std::string, CCommands*>> commandInput;
	static CUserInterface& getInstance();
	char getUserKeyPress(const commandInput commands, char defaultresp);
	~CUserInterface();


private:
	CUserInterface();

	CUserInterface(const CUserInterface&) = delete;
	const CUserInterface& operator= (const CUserInterface &) = delete;


	commandInput mCommands;
};

#endif //SIMPLE_FTP_USER_INTERFACE_HEADER
