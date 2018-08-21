/*
 * =====================================================================================
 *
 *       Filename:  cpy.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/21/2018 01:21:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <string>

int copyFiles (const std::string &file1, const std::string &file2)
{
	int fd_org = 0, fd_dest = 0;

	fd_org  = open(file1.c_str(), O_RDONLY);
	if (-1 == fd_org)
	{
		int err = errno;
		std::cerr << "unable to open files " << file1 << " " << strerror(err) << std::endl;
		return 2;
	}

	struct stat st;
	
	if (-1 == fstat(fd_org, &st))
	{
		int err = errno;
		std::cerr << "unable to stat input file " << strerror(err) << std::endl;
		return 3;
	}

	fd_dest = open(file2.c_str(), O_RDWR | O_CREAT, st.st_mode);
	
	if (-1 == fd_dest)
	{
		int err = errno;
		std::cerr << "unable to open files " << file2 << " " << strerror(err) << std::endl;
		return 2;
	}

	char *addr_dest = nullptr, *addr_org = nullptr;

	addr_org = reinterpret_cast<char*>(mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd_org, 0));
	if (MAP_FAILED == addr_org)
	{
		int err = errno;
		std::cerr << "unable to mmap file in memory " << strerror(err) << std::endl;
		return 4;
	}

	close(fd_org);	

	ftruncate(fd_dest, st.st_size);

	addr_dest = reinterpret_cast<char*> (mmap(NULL, st.st_size, PROT_WRITE, MAP_SHARED, fd_dest, 0));
	
	if (MAP_FAILED == addr_dest)
	{
		int err = errno;
		std::cerr << "unable to mmap file in memory " << strerror(err) << std::endl;
		return 4;
	}
	
	memcpy(addr_dest, addr_org, st.st_size);	
	
	if (-1 == msync(addr_dest, st.st_size, MS_SYNC))
	{
		int err = errno;
		std::cerr << "unable to sync the mem to file " << strerror(err) << std::endl;
		return 5;
	}

	close(fd_dest);
	
	munmap(addr_org, st.st_size);

	munmap(addr_dest, st.st_size);
	return 0;
}
