/*
 * =====================================================================================
 *
 *       Filename:  socket.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  13/08/2018 13:39:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "socket.hpp"
#include "../logging/logging.hpp"

Socket::Socket(Socket&&s): fdsocket(s.fdsocket), mType(s.mType), servinfo(s.servinfo)
{
	s.fdsocket = -1;
	s.servinfo = nullptr;
}
	
Socket::~Socket() 
{ 
	fdclose();
}
	

bool Socket::isValid ()const
{
	if (-1 == fdsocket)
	{
		return false;
	}
	return true;
}	

Socket Socket::operator = (Socket &&s)
{
	std::swap(fdsocket, s.fdsocket);
	std::swap(servinfo, s.servinfo);
	mType = s.mType;
}

void Socket::fdclose()
{
	if (-1 != fdsocket) 
	{
		close(fdsocket);
		if (nullptr != servinfo)
		{
			freeaddrinfo(servinfo);
			servinfo = nullptr;
		}
	}
}

const int Socket::getSocket() const
{
	return fdsocket;
}

size_t Socket::Send(const unsigned char *buffer, size_t size)
{
	if (nullptr != buffer && fdsocket == -1)
	{
		printLog("Incorrect arguments");
		return 0;	
	}
	size_t ret = send(fdsocket, reinterpret_cast<const void*>(buffer), size, 0);
	if (ret == -1)
	{
		printLog("Error sending");
		return 0;
	}
	if (ret != size)
	{
		printLog("Did not send everyhting ", size, " ", ret);
	}
	return ret;
}

size_t Socket::Receive(unsigned char *buffer, size_t max_length)
{
	if (nullptr != buffer && fdsocket == -1)
	{
		printLog("Incorrect arguments");
		return 0;	
	}
	size_t ret = recv(fdsocket, reinterpret_cast<void*>(buffer), max_length, 0);
	if (ret == -1)
	{
		printLog("Error sending");
		return 0;
	}
	return ret;
}

Socket::Socket(): fdsocket(-1), servinfo(nullptr) 
{
}

Socket::Socket(const int &s, Socket_type_t st): fdsocket(s),mType(st),servinfo(nullptr) 
{
}

Socket::Socket(const int &s): fdsocket(s), mType(SERVER),servinfo(nullptr) 
{
}

Socket::Socket(const std::string &ip, const std::string &port):fdsocket(-1)
{
	if (ip.size() == 0 && port.size() == 0)
	{
		printLog("Error! No port or IP!");
		return;
	}	

	int status;
	struct addrinfo hints;
	struct addrinfo *p;

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	if ((status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo)) != 0) 
	{
    	printLog("getaddrinfo error:", gai_strerror(status));
	    return;
	}
				
	int	s = socket(servinfo->ai_family, servinfo->ai_socktype | SOCK_CLOEXEC, servinfo->ai_protocol);
	if (-1 == s)
	{
		printLog(strerror(errno));
		return;
	}	
	printLog("Socket returned ", s);
	fdsocket = s;
	printLog(*this);
}

Socket Socket::operator = (const int&s)
{
	fdclose();
	fdsocket = s;
	mType = CLIENT;
}

std::ostream & operator << (std::ostream &st, const Socket &s)
{
	st << "Socket value "<< s.fdsocket << " Socket type " << (s.mType == Socket::SERVER ? "Server":"Client") << " Serv info " << (s.servinfo == nullptr? "nullptr": (void*)s.servinfo)	;
	return st;
}

Socket startServer(const std::string &ip, const std::string &port)
{
	Socket s(ip, port);
	printLog(s);
	if (-1 == s.fdsocket)
	{
		printLog("Invalid socket");
		return Socket(-1);
	}
	int ret = bind(s.fdsocket, s.servinfo->ai_addr, s.servinfo->ai_addrlen);		
	if (-1 == ret)
	{
		printLog(strerror(errno));
		return Socket(-1);
	}
	printLog("bind returned ", ret);

	ret = listen(s.fdsocket, 10);
	if (-1 == ret)
	{
		printLog(strerror(errno));
		return Socket(-1);
	}
	printLog("listen returned ", ret);

	s.mType = Socket::SERVER;
	printLog(s);
	return s;	
}

Socket ConnectToServer(const std::string &ip, const std::string &port)
{
	Socket s(ip, port);
	if (-1 == s.fdsocket)
	{
		return -1;
	}
	
	int ret = connect(s.fdsocket, s.servinfo->ai_addr, s.servinfo->ai_addrlen);

	if (-1 == ret)
	{
		printLog(strerror(errno));
		return -1;
	}
	printLog(s);
	return s;
}

Socket getNewClientConnection(const Socket &s)
{
	if (s.fdsocket == -1 || s.mType != Socket::SERVER)
	{
		printLog("Invalid Socket ");
		return -1;
	}
	struct sockaddr_storage their_addr;	
	socklen_t addr_size = sizeof their_addr;
	int new_fd = accept(s.fdsocket, (struct sockaddr *)&their_addr, &addr_size);
	if (-1 == new_fd)
	{
		printLog(strerror(errno));
		return -1;
	}
	printLog("accept returned ", new_fd);	
	
	return Socket(new_fd, Socket::CLIENT);
}
