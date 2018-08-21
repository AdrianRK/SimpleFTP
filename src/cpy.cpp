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

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
	if (argc < 2)
	{
		std::cerr<< "incorrect usage" << std::endl;
		return 1;
	}
	int fd_org = 0, fd_dest = 0;

	fd_org  = open(argv[1], O_RDONLY);
	if (-1 == fd_org)
	{
		int err = errno;
		std::cerr << "unable to open files " << argv[1] << " " << strerror(err) << std::endl;
		return 2;
	}

	struct stat st;
	
	if (-1 == fstat(fd_org, &st))
	{
		int err = errno;
		std::cerr << "unable to stat input file " << strerror(err) << std::endl;
		return 3;
	}

	fd_dest = open(argv[2], O_RDWR | O_CREAT, st.st_mode);
	
	if (-1 == fd_dest)
	{
		int err = errno;
		std::cerr << "unable to open files " << argv[2] << " " << strerror(err) << std::endl;
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

	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
