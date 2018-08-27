/*
 * =====================================================================================
 *
 *       Filename:  ui.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/27/2018 09:42:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "../inc/ui.hpp"
#include "../inc/logging.hpp"
#include <iostream>

CUserInterface& CUserInterface::getInstance()
{
	static CUserInterface ui;
	return ui;
}

CUserInterface::~CUserInterface()
{
}

CUserInterface::CUserInterface()
{

}

void printTitle()
{
	std::cout << "************************************************\n";
	std::cout << "*                                              *\n";
	std::cout << "*              User Interface                  *\n";
	std::cout << "*                                              *\n";
	std::cout << "************************************************\n";
}

char CUserInterface::getUserKeyPress(const commandInput commands, char defaultresp)
{
	printLog("Enter function");
	if (0 == commands.size())
	{
		return defaultresp;
	}
	printLog("Entering curse mode");
	printf("\033[H\033[J");
	bool exitFlag = false;		
	
	while(!exitFlag)
	{
		//printf("\033[H\033[J");
		printTitle();
		for (auto it : commands)
		{
			std::cout << it.second.first << "\n";
		}
		std::cout << "Press " << defaultresp<< " for exit\n";
		char c = 0;
		std::cout << "Enter option: ";
		std::cin >> c;
		std::cout<< "You have pressed " << c << "\n";
		if (c == defaultresp)
		{
			break;
		}

		for (auto it : commands)
		{
			if (c == it.first)
			{
				std::cout << "Command found " << c << " printing message: " << it.second.first << "\n";
				it.second.second->operator()();
			}
		}
	}
	
	return defaultresp;
}
