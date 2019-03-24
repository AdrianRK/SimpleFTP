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

#include "tools.hpp"
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <cstring>
#include <sstream>
#include <dirent.h>

CMapedMem::~CMapedMem()
{
	if (nullptr != mData)	
	{
		munmap(mData, mLength);
		mData = nullptr;
	}
}

CMapedMem::CMapedMem(CMapedMem&&obj)
{
	mData = obj.mData;
	obj.mData = nullptr;
	mLength = obj.mLength;
	mMode = obj.mMode;
}

CMapedMem::CMapedMem(void *data, size_t length, mode_t mode): mData(data), mLength(length), mMode(mode)
{
}

CMapedFile::CMapedFile(CMapedFile&&obj): CMapedMem(std::move(obj)) {}


CMapedFile::CMapedFile(void * data, size_t length): CMapedMem(data, length) {}

CMapedFile::~CMapedFile()
{
	if (nullptr != mData)
	{
		if (-1 == msync(mData, mLength, MS_SYNC))
		{
			int err = errno;
			std::cerr << "unable to sync the mem to file " << strerror(err) << std::endl;
		}
	}
}

CMapedMem loadFileFromDisk(const std::string&file)
{
	CMapedMem mem(nullptr, 0);
	
	int fd_org = 0;

	fd_org  = open(file.c_str(), O_RDONLY);
	if (-1 == fd_org)
	{
		int err = errno;
		std::cerr << "unable to open files " << file << " " << strerror(err) << std::endl;
		return mem;
	}

	struct stat st;
	
	if (-1 == fstat(fd_org, &st))
	{
		int err = errno;
		std::cerr << "unable to stat input file " << strerror(err) << std::endl;
		return mem;
	}
	
	unsigned char *addr_org = nullptr;

	addr_org = reinterpret_cast<unsigned char*>(mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd_org, 0));
	if (MAP_FAILED == addr_org)
	{
		int err = errno;
		std::cerr << "unable to mmap file in memory " << strerror(err) << std::endl;
		return mem;
	}

	close(fd_org);

	mem.mData = addr_org;
	mem.mLength = st.st_size;
	mem.mMode = st.st_mode;
	
	return mem;
}

int copyFiles (const std::string &file1, const std::string &file2)
{
	CMapedMem mem(loadFileFromDisk(file1));
	
	saveFileToDisk(mem, file2);

	return 0;
}

int saveFileToDisk(void* data, size_t length, const std::string &file, const mode_t & mode)
{
	if (nullptr == data || 0 == file.size())	
	{
		return 0;
	}

	int fd_dest = 0;

	fd_dest = open(file.c_str(), O_RDWR | O_CREAT, mode);
	
	if (-1 == fd_dest)
	{
		int err = errno;
		std::cerr << "unable to open files " << file << " " << strerror(err) << std::endl;
		return 1;
	}
	char *addr_dest = nullptr;
	ftruncate(fd_dest, length);

	addr_dest = reinterpret_cast<char*> (mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd_dest, 0));
	
	if (MAP_FAILED == addr_dest)
	{
		int err = errno;
		std::cerr << "unable to mmap file in memory " << strerror(err) << std::endl;
		return 3;
	}
	
	memcpy(addr_dest, data, length);	
	
	if (-1 == msync(addr_dest, length, MS_SYNC))
	{
		int err = errno;
		std::cerr << "unable to sync the mem to file " << strerror(err) << std::endl;
		return 4;
	}

	close(fd_dest);

	munmap(addr_dest, length);

	return 0;	
}


int saveFileToDisk(const CMapedMem & mem, const std::string &file)
{
	return saveFileToDisk(mem.getBuffer(), mem.getLength(), file, mem.getMode());
}

CMapedFile mapNewFile(const std::string&file, size_t size, mode_t mode)
{
	CMapedFile mem (nullptr, 0);
	int fd_dest = open(file.c_str(), O_RDWR | O_CREAT, mode);
	
	if (-1 == fd_dest)
	{
		int err = errno;
		std::cerr << "unable to open files " << file << " " << strerror(err) << std::endl;
		return mem;
	}
	char *addr_dest = nullptr;
	ftruncate(fd_dest, size);

	addr_dest = reinterpret_cast<char*> (mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd_dest, 0));
	
	if (MAP_FAILED == addr_dest)
	{
		int err = errno;
		std::cerr << "unable to mmap file in memory " << strerror(err) << std::endl;
		return mem;
	}

	mem.mData = addr_dest;
	mem.mLength = size;
	mem.mMode = mode;
	return mem;
}


size_t getFileSize(const std::string &fileName)
{
	int fd = open(fileName.c_str(),O_RDONLY);
	struct stat st;
	if (-1 == fstat(fd, &st))
	{
		int err = errno;
		std::cerr << "unable to stat input file " << strerror(err) << std::endl;
		return 0;
	}
	return st.st_size;
}

std::string getListOfFiles(const std::string& fileLocation)
{
	std::stringstream ss;
	DIR *d;
  	struct dirent *dir;
 	d = opendir(fileLocation.c_str());
  	if (d) 
	{
    	while ((dir = readdir(d)) != NULL) 
		{
			std::string str = dir->d_name;
			if ((str != ".") && (str != ".."))
			{
				size_t fileSize = getFileSize(fileLocation + str);
      			ss << "File name is: " << str << " of size " << fileSize << "\n";
			}
    	}
    	closedir(d);
  	}
	return ss.str();
}
