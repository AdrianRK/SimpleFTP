/*
 * =====================================================================================
 *
 *       Filename:  simpleftp.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  13/08/2018 14:16:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef SIMPLE_FILE_TRANSFER_PROTOCOL_HEADER
#define SIMPLE_FILE_TRANSFER_PROTOCOL_HEADER	

namespace simpleftp
{

class interface
{
public:
	virtual size_t Send(const unsigned char *buffer, size_t size) = 0;
	virtual size_t Receive(unsigned char *buffer, size_t size) = 0;
	virtual size_t getBufferSize() const = 0;
	virtual ~interface() = default;
};

void _transferBuffer(interface & intf, const unsigned char*buffer, size_t size);
unsigned char* _receiveBuffer(interface &intf, unsigned char* buffer, size_t max_len);
} // namespace simpleftp
#endif ///SIMPLE_FILE_TRANSFER_PROTOCOL_HEADER
