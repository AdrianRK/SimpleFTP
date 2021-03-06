/*
 * =====================================================================================
 *
 *       Filename:  protocol.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/27/2018 03:15:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
//#include "config.hpp"
#include "tools.hpp"
//#include "logging.hpp"
#include "simpleftp.hpp"
#include <arpa/inet.h>

namespace simpleftp
{

void SendBuffer(interface &s, const unsigned char * buffer, size_t size)
{
	unsigned char nsize[4] = {0,};
	nsize[0] = htonl(int32_t(size)) >> 24;
	nsize[1] = htonl(int32_t(size)) >> 16;
	nsize[2] = htonl(int32_t(size)) >> 8;
	nsize[3] = htonl(int32_t(size));
	//printLog(int(nsize[0]), " ", int(nsize[1]), " ", int(nsize[2]), " ", int(nsize[3]));
	
	s.Send(nsize, sizeof(nsize));
	_transferBuffer(s, buffer, size);
}

unsigned char* ReceiveBuffer(interface &s, size_t &size)
{
	int32_t len;
	unsigned char nsize[4] = {0,};
	s.Receive(nsize, sizeof(nsize));
//	printLog(int(nsize[0]), " ", int(nsize[1]), " ", int(nsize[2]), " ", int(nsize[3]));
	len = ntohl(nsize[3] + (static_cast<unsigned long>(nsize[2]) << 8) + (static_cast<unsigned long>(nsize[1]) << 16) + (static_cast<unsigned long>(nsize[0]) << 24));
//	printLog("Size of expected message is ", len);

	unsigned char * buffer = _receiveBuffer(s, nullptr, len);

	size = len;
	return buffer;
}

void SendFile(interface &s, const std::string&fileName)
{
	CMapedMem mem (loadFileFromDisk(/*CConfig::getInstance().getStringParameter("FTPLocation") + */fileName));

//	printLog("Sending buffer of size ", mem.getLength());
	
	SendBuffer(s, reinterpret_cast<const unsigned char*>(mem.getBuffer()), mem.getLength());
}

void ReceiveFile(interface &s, const std::string&fileName)
{
	unsigned char * buffer = nullptr;
	int32_t len;
	unsigned char nsize[4] = {0,};
	s.Receive(nsize, sizeof(nsize));
//	printLog(int(nsize[0]), " ", int(nsize[1]), " ", int(nsize[2]), " ", int(nsize[3]));
	len = ntohl(nsize[3] + (static_cast<unsigned long>(nsize[2]) << 8) + (static_cast<unsigned long>(nsize[1]) << 16) + (static_cast<unsigned long>(nsize[0]) << 24));
//	printLog("Size of expected message is ", len);

	CMapedFile mem (mapNewFile(/*CConfig::getInstance().getStringParameter("FTPLocation") + */fileName, len));
	buffer = _receiveBuffer(s, reinterpret_cast<unsigned char*>(mem.getBuffer()), mem.getLength());
//	printLog("Received buffer of size ", len);
	if (nullptr == mem.getBuffer())
	{
		if (nullptr != buffer)
		{
			saveFileToDisk(reinterpret_cast<void*>(buffer),len,fileName);
			delete [] buffer;	
		}
	}
}

void ProtocolServer(interface& s, std::string location)
{
	bool quitFlag = false;

	while(!quitFlag)
	{
		char c = 0;
		s.Receive(reinterpret_cast<unsigned char *>(&c), 1);
		switch (c)
		{
			case 'd':
			{
				size_t size = 0;
				unsigned char *buffer = ReceiveBuffer(s, size);
//				printLog("Receive filename ", size);
				if (nullptr != buffer)
				{
					SendFile(s, reinterpret_cast<char*>(buffer));
					delete [] buffer;
				}
				break;
			}
			case 'u':
			{
				size_t size = 0;
				unsigned char *buffer = ReceiveBuffer(s, size);
//				printLog("Receive filename ", size);
				if (nullptr != buffer)
				{
					ReceiveFile(s, reinterpret_cast<char*>(buffer));
				}
				break;
			}
			case 'l':
			{
				//std::string str = getListOfFiles(CConfig::getInstance().getStringParameter("FTPLocation"));
				std::string str = location;
				SendBuffer(s, reinterpret_cast<const unsigned char*>(str.c_str()), str.size());								

//				printLog("Sending directory list");
				break;
			}
			case 'q':
			default:
				quitFlag = true;
				break;
		}
	}
}

void ProtocolClient(interface& s, char command, const std::string & fileName, std::string &fileList)
{
	switch (command)
	{
		case 'd':
			s.Send(reinterpret_cast<unsigned char*>(&command), 1);
			SendBuffer(s,reinterpret_cast<const unsigned char*>(fileName.c_str()), fileName.size());
			ReceiveFile(s, fileName);
			break;
		case 'u':
			s.Send(reinterpret_cast<unsigned char*>(&command), 1);
			SendBuffer(s,reinterpret_cast<const unsigned char*>(fileName.c_str()), fileName.size());
			SendFile(s, fileName);
			break;
		case 'l':
			{
			s.Send(reinterpret_cast<unsigned char*>(&command), 1);
			size_t size = 0;		
			unsigned char * buffer = ReceiveBuffer(s, size);
			if (nullptr != buffer)
			{
				fileList = reinterpret_cast<char*>(buffer);	
				delete[] buffer;	
			}
			break;
			}
		default:
			break;
	}
}
} // namespace simpleftp
