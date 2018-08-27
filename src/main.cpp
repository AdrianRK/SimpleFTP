/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/08/2018 11:22:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "../inc/threadpool.hpp"
#include "../inc/logging.hpp"
#include "../inc/socket.hpp"
#include "../inc/config.hpp"
#include "../inc/ui.hpp"
#include "../inc/protocol.hpp"
#include "../inc/tools.hpp"
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

class processJob
{
public:
	processJob(Socket&& s): mSk(std::move(s)) {}
	processJob(const processJob & jb): mSk(std::move(const_cast<processJob&>(jb).mSk)) {}
	processJob(processJob && jb): mSk(std::move(jb.mSk)) {}

	void operator()() 
	{
		printLog(mSk);
		ProtocolServer(mSk);
	}
private:
	Socket mSk;
};

class downloadFile: public CCommands
{
public:
	downloadFile(Socket& s): mSk(s) {}

	virtual void operator()() 
	{
		std::cout << "Download file\nEnter File Name:";
		std::string fileName;
		std::cin >> fileName;
		std::string list;
		ProtocolClient(mSk, 'd', fileName, list);	
	}
private:
	Socket &mSk;
};

class uploadFile: public CCommands
{
public:
	uploadFile(Socket& s): mSk(s) {}
	virtual void operator()() 
	{
		std::cout << "Upload file\nEnter File Name:";
		std::string fileName;
		std::cin >> fileName;
		std::string list;
		ProtocolClient(mSk, 'u', fileName, list);
	}
private:
	Socket &mSk;
};

class getFileList: public CCommands
{
public:
	getFileList(Socket& s): mSk(s) {}
	virtual void operator()() 
	{
		std::cout << "Get file list\n";
		std::string list;
		ProtocolClient(mSk, 'l', "", list);
		std::cout << "Printing File list:\n";
		std::cout << list << "\n";
	}
private:
	Socket &mSk;
};

class printLocalFileList: public CCommands
{
public:
	virtual void operator()() 
	{
		std::cout << "Printing local file list\n";
		std::string list = getListOfFiles(CConfig::getInstance().getParameter("FTPLocation"));
		std::cout << "Printing File list:\n";
		std::cout << list << "\n";
	}
};

int main(int argc, char **argv)
{
	printLog(CConfig::getInstance().getParameter("FTPLocation"));
		
	if (argc < 2)
	{
		return 1;
	}

	if (std::string(argv[1]) == "Server" || std::string(argv[1]) == "server")
	{
		printLog("Starting up a server");
		if (argc < 4)
		{
			printLog("Invalid number of parameters");
			return 1;
		}
		printLog("Starting server with IP ", argv[2], " and Port ", argv[3]);
	
		ThreadPool<processJob> serverPool;
		Socket server = startServer(argv[2], argv[3]);
		printLog(server);
		if(server.isValid())
		{
			while(1)
			{
				printLog("Get next client");
				Socket s = getNewClientConnection(server);
				printLog("New client connection on", s);
				processJob job(std::move(s));
				serverPool.postJob(job);
			}
		}
	}
	
	if (std::string(argv[1]) == "Client" || std::string(argv[1]) == "client")
	{
		printLog("Starting up a client");
		if (argc < 4)
		{
			printLog("Invalid number of parameters");
			return 1;
		}
		printLog("Starting server with IP ", argv[2], " and Port ", argv[3]);
		
		Socket s = ConnectToServer(argv[2], argv[3]);
		printLog(s);
		
		CUserInterface::commandInput cmlst;
		cmlst['1'] = std::pair<std::string, CCommands*>("1. Download file from server", new downloadFile(s));	
		cmlst['2'] = std::pair<std::string, CCommands*>("2. Upload file to server", new uploadFile(s));	
		cmlst['3'] = std::pair<std::string, CCommands*>("3. Get list of files on server", new getFileList(s));
		cmlst['4'] = std::pair<std::string, CCommands*>("4. Print local list of files", new printLocalFileList);
		CUserInterface::getInstance().getUserKeyPress(cmlst, 'q');
		const unsigned char c = 'q';
		s.Send(&c,1);
	}

	return 0;
}
