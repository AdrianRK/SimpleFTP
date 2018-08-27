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

#include "../inc/tools.hpp"
#include "../inc/logging.hpp"
#include "../inc/socket.hpp"

void SendFile(Socket &s, const std::string&fileName)
{
	CMapedMem mem (loadFileFromDisk(fileName));

	printLog("Sending buffer of size ", mem.getLength());
	unsigned char nsize[4] = {0,};
	nsize[0] = htonl(int32_t(mem.getLength())) >> 24;
	nsize[1] = htonl(int32_t(mem.getLength())) >> 16;
	nsize[2] = htonl(int32_t(mem.getLength())) >> 8;
	nsize[3] = htonl(int32_t(mem.getLength()));
	printLog(int(nsize[0]), " ", int(nsize[1]), " ", int(nsize[2]), " ", int(nsize[3]));
	
	s.Send(nsize, sizeof(nsize));
	
	transferFile(s, reinterpret_cast<unsigned char*>(mem.getBuffer()), mem.getLength());
}

void ReceiveFile(Socket &s, const std::string&fileName)
{
	unsigned char * buffer = nullptr;
	int32_t len;
	unsigned char nsize[4] = {0,};
	s.Receive(nsize, sizeof(nsize));
	printLog(int(nsize[0]), " ", int(nsize[1]), " ", int(nsize[2]), " ", int(nsize[3]));
	len = ntohl(nsize[3] + (static_cast<unsigned long>(nsize[2]) << 8) + (static_cast<unsigned long>(nsize[1]) << 16) + (static_cast<unsigned long>(nsize[0]) << 24));
	printLog("Size of expected message is ", len);

	CMapedFile mem (mapNewFile(fileName, len));
	buffer = receiveFile(s, reinterpret_cast<unsigned char*>(mem.getBuffer()), mem.getLength());
	printLog("Received buffer of size ", len);
	if (nullptr == mem.getBuffer())
	{
		if (nullptr != buffer)
		{
			saveFileToDisk(reinterpret_cast<void*>(buffer),len,fileName);
			delete [] buffer;	
		}
	}
}

void ProtocolServer(Socket& s)
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
				unsigned char buffer[256] = {0,};
				size_t ret = s.Receive(buffer, 255);
				printLog("Receive filename ", ret);
				SendFile(s, reinterpret_cast<char*>(buffer));
				break;
			}
			case 'u':
			{
				unsigned char buffer[256] = {0,};
				size_t ret = s.Receive(buffer, 255);
				printLog("Receive filename ", ret);
				ReceiveFile(s, reinterpret_cast<char*>(buffer));
				break;
			}
			case 'l':
			{
				break;
			}
			case 'q':
			default:
				quitFlag = true;
				break;
		}
	}
}

void ProtocolClient(Socket& s, char command, const std::string & fileName, std::string &fileList)
{
	switch (command)
	{
		case 'd':
			s.Send(reinterpret_cast<unsigned char*>(&command), 1);
			s.Send(reinterpret_cast<const unsigned char*>(fileName.c_str()), fileName.size());
			ReceiveFile(s, fileName);
			break;
		case 'u':
			s.Send(reinterpret_cast<unsigned char*>(&command), 1);
			s.Send(reinterpret_cast<const unsigned char*>(fileName.c_str()), fileName.size());
			SendFile(s, fileName);
			break;
		case 'l':
			s.Send(reinterpret_cast<unsigned char*>(&command), 1);
		default:
			break;
	}
}
