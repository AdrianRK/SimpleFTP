/*
 * =====================================================================================
 *
 *       Filename:  tools.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/22/2018 09:53:02 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef SIMPLE_FTP_TOOLS_HEADER
#define SIMPLE_FTP_TOOLS_HEADER

#include <fcntl.h>
#include <string>

class CMapedMem
{
public:
	virtual ~CMapedMem();
	void * getBuffer() const
	{
		return mData;
	}
	size_t getLength() const 
	{
		return mLength;
	}
	mode_t getMode() const
	{
		return mMode;
	}

	CMapedMem(CMapedMem&&);

protected:
	explicit CMapedMem(void *data, size_t length, mode_t mode = S_IRUSR | S_IWUSR);

	const CMapedMem &operator=(CMapedMem&) = delete;
	CMapedMem() = delete;
	CMapedMem(const CMapedMem&) = delete;

	void * mData;
	size_t mLength;
	mode_t mMode;

friend CMapedMem loadFileFromDisk(const std::string&file);
};

class CMapedFile: public CMapedMem
{
public:
	~CMapedFile();
	CMapedFile(CMapedFile&&);
private:
	explicit CMapedFile(void *data, size_t length);

friend CMapedFile mapNewFile(const std::string&file, size_t size, mode_t mode);
};

CMapedFile mapNewFile(const std::string&file, size_t size, mode_t mode = S_IRUSR | S_IWUSR);
CMapedMem loadFileFromDisk(const std::string&file);
int copyFiles (const std::string &file1, const std::string &file2);
int saveFileToDisk(void* data, size_t length, const std::string &file, const mode_t & mode = 0);
int saveFileToDisk(const CMapedMem & mem, const std::string &file);



#endif // SIMPLE_FTP_TOOLS_HEADER
