/*
 * =====================================================================================
 *
 *       Filename:  simpleftp.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  13/08/2018 14:23:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "../inc/simpleftp.hpp"
#include "../inc/logging.hpp"
#include <cstring>
#include <arpa/inet.h>

const static size_t DATA_CHUNK = 2048;
void transferFile(interface & intf, const unsigned char* buffer, size_t size)
{
	if (nullptr != buffer && size != 0)
	{
		printLog("Sending buffer of size ", size);
		unsigned char nsize[2] = {0,};
		unsigned short shsize = size;
		nsize[0] = htons(shsize) >> 8;
		nsize[1] = htons(shsize);
		intf.Send(nsize, sizeof(nsize));
		printLog(int(nsize[0]), " ", int(nsize[1]));	
		size_t nrOfPackets = size / DATA_CHUNK + (size % DATA_CHUNK == 0 ? 0:1);
		size_t ret = 0;
		unsigned char rep = 0;
		for (size_t i = 0; i < nrOfPackets; ++i)	
		{
			printLog("Sending packet nr ", i);
			unsigned char buff[25] = {0,};
			if (i + 1 < nrOfPackets)
			{
				ret = intf.Send(buffer + (i * DATA_CHUNK), DATA_CHUNK);
				if (ret != DATA_CHUNK)
				{
					printLog("Error sending data chunk nr ", i);
					rep = 'n';
					intf.Send(&rep,1);
					break;
				}
			}
			else
			{
				ret = intf.Send(buffer + (i * DATA_CHUNK), size % DATA_CHUNK);
				if (ret != size % DATA_CHUNK)
				{
					printLog("Error sending data chunk nr ", i);
					rep = 'n';
					intf.Send(&rep,1);
					break;
				}
			}
			intf.Receive(buff, 24);
			switch (buff[0])
			{
				case 'k':
					continue;
				case 'n':
					break;
				default:
					break;
			}
			break;
		}
	}
}

unsigned char * receiveFile(interface &intf, unsigned char *buffer, size_t &size)
{
	printLog("Receiving files");
	size = 0;
	size_t ret = 0;
	unsigned char rep = 0;
	unsigned char nsize[2] = {0,};
	intf.Receive(nsize, sizeof(nsize));
	printLog(int(nsize[0]), " ", int(nsize[1]));
	size = ntohs(nsize[1] + (static_cast<unsigned short>(nsize[0]) << 8));
	size_t nrOfPackets = (size / DATA_CHUNK) + (size % DATA_CHUNK == 0 ? 0:1);
	printLog("Size of expected message is ", size);
	if (nullptr == buffer)	
	{
		buffer = new unsigned char [size+1];
		memset(buffer, 0, sizeof(unsigned char) * (size+1));
	}
	printLog("Expected nr of packets ", nrOfPackets);
	for (size_t i = 0; i < nrOfPackets; ++i)	
	{
		printLog("Receiving packet nr ", i);
		if (i + 1 < nrOfPackets)
		{
			printLog("Expecting data chunk of size ", DATA_CHUNK);
			ret = intf.Receive(buffer + (i * DATA_CHUNK), DATA_CHUNK);
			if (ret != DATA_CHUNK)
			{
				printLog("Error sending data chunk nr ", i);
				rep = 'n';
				intf.Send(&rep,1);
				break;
			}
		}
		else
		{
			printLog("Expecting dada chunk of size ", size % DATA_CHUNK);
			ret = intf.Receive(buffer + (i * DATA_CHUNK), size % DATA_CHUNK);
			if (ret != size % DATA_CHUNK)
			{
				printLog("Error sending data chunk nr ", i);
				rep = 'n';
				intf.Send(&rep,1);
				break;
			}			
		}
		printLog("Received data chunk of size ", ret);
		rep = 'k';
		intf.Send(&rep, 1);
	}
	return buffer;
}
