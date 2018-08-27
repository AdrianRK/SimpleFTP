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



void _transferBuffer(interface & intf, const unsigned char* buffer, size_t size)
{
	if (nullptr != buffer && size != 0)
	{
		const size_t DATA_CHUNK = intf.getBufferSize();
		size_t nrOfPackets = size / DATA_CHUNK + (size % DATA_CHUNK == 0 ? 0:1);
		size_t retryCount = 5;
		printLog("Number of packets is ", nrOfPackets);
		int ret = 0;
		for (size_t i = 0; i < nrOfPackets;)	
		{
			printLog("Sending packet nr ", i);
			unsigned char buff = 0;
			retryCount = 5;
			if (i + 1 < nrOfPackets)
			{
				printLog("Sending data packet of size ",DATA_CHUNK);
				ret = intf.Send(buffer + (i * DATA_CHUNK), DATA_CHUNK);
				printLog("Sent data chunk of size ", ret);
				if (ret != int(DATA_CHUNK))
				{
					printLog("Error sending data chunk nr ", i);
				}
			}
			else
			{	
				printLog("Sending data packet of size ",size % DATA_CHUNK);
				ret = intf.Send(buffer + (i * DATA_CHUNK), size % DATA_CHUNK);
				printLog("Sent data chunk of size ", ret);
				if (ret != int(size % DATA_CHUNK))
				{
					printLog("Error sending data chunk nr ", i);
				}
			}
			intf.Receive(&buff, 1);
			switch (buff)
			{
				case 'k':
					++i;
					retryCount = 5;
				continue;
				case 'n':
					if (!retryCount--)	
					{
						break;
					}
					else
					{
						continue;
					};
				default:
					break;
			}
			break;
		}
	}
}

unsigned char * _receiveBuffer(interface &intf, unsigned char *buffer, size_t size)
{
	printLog("Receiving buffer of size ", size);
	size_t ret = 0;
	unsigned char rep = 0;
	size_t retryCount = 5;
	const size_t DATA_CHUNK = intf.getBufferSize();;
	if (nullptr == buffer)	
	{
		printLog("buffer is null");
		buffer = new unsigned char [size+1];
		memset(buffer, 0, sizeof(unsigned char) * (size+1));
	}
	size_t nrOfPackets = (size / DATA_CHUNK) + (size % DATA_CHUNK == 0 ? 0:1);
	printLog("Expected nr of packets ", size, " ", nrOfPackets, " ", (size / DATA_CHUNK), " ", (size % DATA_CHUNK == 0 ? 0:1));
	for (size_t i = 0; i < nrOfPackets;)	
	{
		printLog("Receiving packet nr ", i);
		if (i + 1 < nrOfPackets)
		{
			printLog("Expecting data chunk of size ", DATA_CHUNK);
			ret = intf.Receive(buffer + (i * DATA_CHUNK), DATA_CHUNK);
			printLog("Received data chunk of size ", ret);
			if (ret != DATA_CHUNK)
			{
				printLog("Error receiving data chunk nr ", i);
				if (!retryCount--)
				{
					rep = 'n';
					intf.Send(&rep,1);
					break;
				}
				else
				{
					continue;
				}
			}
		}
		else
		{
			printLog("Expecting dada chunk of size ", size % DATA_CHUNK);
			ret = intf.Receive(buffer + (i * DATA_CHUNK), size % DATA_CHUNK);
			printLog("Received data chunk of size ", ret);			
			if (ret != size % DATA_CHUNK)
			{
				printLog("Error receiving data chunk nr ", i);
				if (!retryCount--)
				{
					rep = 'n';
					intf.Send(&rep,1);
					break;
				}
				else
				{
					continue;
				}
			}
		}
		printLog("Received data chunk of size ", ret);
		rep = 'k';
		intf.Send(&rep, 1);
		retryCount = 5;
		++i;
	}
	return buffer;
}
