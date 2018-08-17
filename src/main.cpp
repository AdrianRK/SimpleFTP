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

#include "../ThreadPool/threadpool.hpp"
#include "../logging/logging.hpp"
#include "socket.hpp"
#include <fstream>


void SendFile(Socket &s, const std::string&fileName)
{
	std::ifstream is;
	printLog(fileName);
	is.open (fileName.c_str(), std::ios::binary);
	// get length of file:
	is.seekg (0, std::ios::end);
	size_t length = is.tellg();
	is.seekg (0, std::ios::beg);
	printLog(length);
	// allocate memory:
	unsigned char *buffer = new unsigned char [length];
	is.read(reinterpret_cast<char*>(buffer),length);
	is.close();
	transferFile(s, buffer, sizeof(unsigned char)*length);
	delete [] buffer;
}

void ReceiveFile(Socket &s, const std::string&fileName)
{
	unsigned char * buffer = nullptr;
	size_t len;
	buffer = receiveFile(s, buffer, len);
	printLog("Received buffer of size", len);
	std::ofstream fs;
	fs.open(fileName.c_str(), std::ios::binary);	
	if (nullptr != buffer)	
	{	
		printLog("Writing buffer of size ", len);
		fs.write(reinterpret_cast<char*>(buffer), len * sizeof(unsigned char));
		fs.flush();
	}
	fs.close();
	printLog("Received length ", len);
	if(buffer != nullptr)
	{
		delete [] buffer;
	}
}


class processJob
{
public:
	processJob(Socket&& s): mSk(std::move(s)) {}
	processJob(const processJob & jb): mSk(std::move(const_cast<processJob&>(jb).mSk)) {}
	processJob(processJob && jb): mSk(std::move(jb.mSk)) {}

	void operator()() 
	{
		printLog(mSk);
		//std::this_thread::sleep_for(std::chrono::seconds(5));
		/*unsigned char buffer[256] = {0,};
		size_t len = 0;
		while(1)
		{
			memset(buffer, 0 , 256 * sizeof(unsigned char));
			len = mSk.Receive(buffer, 255);
			printLog("Received len ", len);
			if (0 == len)
			{
				break;
			}
			printLog(reinterpret_cast<char*>(buffer));
			if (std::string (reinterpret_cast<char*>(buffer)) == "exit\n")
			{
				printLog("Client requested exit");
				break;
			}
			len = mSk.Send(buffer, len);
			if (0 == len)
			{
				break;
			}printLog("Sent len ", len);
		}*/
		/*unsigned char command = 0;
		bool fexit = false;
		do
		{	
			s.Receive(&command, 1);
			switch (command)
			{
				case 'u':
					break;
				case 'd':
					break;
				case 'l':
					break;
				case 'e':
					break;
				default:
					fexit = false;
					break;
			}
		}while(exit);*/
		unsigned char buffer[256] = {0,};
		size_t ret = mSk.Receive(buffer, 255);
		if (0 != ret)
		{
			ReceiveFile(mSk, reinterpret_cast<char*>(buffer));
		}
	}
private:
	Socket mSk;
};


int main(int argc, char **argv)
{

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
		if (argc < 5)
		{
			printLog("Invalid number of parameters");
			return 1;
		}
		printLog("Starting server with IP ", argv[2], " and Port ", argv[3]);
		
		Socket s = ConnectToServer(argv[2], argv[3]);
		printLog(s);
		s.Send(reinterpret_cast<unsigned char*>(argv[4]), strlen(argv[4]));
		SendFile(s,argv[4]);		
	}

	return 0;
}
