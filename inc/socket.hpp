/*
 * =====================================================================================
 *
 *       Filename:  socket.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  13/08/2018 13:36:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SOCKET_HEADER_DEFINITION
#define SOCKET_HEADER_DEFINITION
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string>
#include <unistd.h>

#include "simpleftp.hpp"

class Socket: public interface
{
public:
	Socket(Socket&&s);
	~Socket();

	Socket operator = (const Socket&s) = delete;

	bool isValid ()const;
	
	Socket operator = (Socket &&s);

	void fdclose();
	

	const int getSocket() const;

	size_t Send(const unsigned char *buffer, size_t size);

	size_t Receive(unsigned char *buffer, size_t max_length);

private:
	typedef enum {SERVER, CLIENT} Socket_type_t;
	
	Socket();
	Socket(const int &s, Socket_type_t st);
	Socket(const int &s);
	Socket(const Socket &) = delete;
	Socket(const std::string &ip, const std::string &port);
	Socket operator = (const int&s);
	int fdsocket;
	Socket_type_t mType;
	struct addrinfo *servinfo;  // will point to the results

public:
	friend Socket startServer(const std::string &ip, const std::string &port);
	friend Socket ConnectToServer(const std::string &ip, const std::string &port);
	friend Socket getNewClientConnection(const Socket &s);
	friend std::ostream & operator << (std::ostream &st, const Socket &s);
};

std::ostream & operator << (std::ostream &st, const Socket &s);
Socket startServer(const std::string &ip, const std::string &port);
Socket ConnectToServer(const std::string &ip, const std::string &port);
Socket getNewClientConnection(const Socket &s);
#endif ////SOCKET_HEADER_DEFINITION
