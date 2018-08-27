/*
 * =====================================================================================
 *
 *       Filename:  logging.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/08/2018 12:20:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef SIMPLE_FTP_LOGGING_HEADER_
#define SIMPLE_FTP_LOGGING_HEADER_
#include <iostream>
#include <fstream>
template <typename T>
void log(std::ostream &stream, const T & obj)
{
	stream << obj << "\n";
}

template <typename T, typename... agcs>
void log (std::ostream &stream, const T& obj, const agcs&... arg)
{
	stream << obj;
	log(stream, arg...);
}

#ifdef D_DEBUG

static std::ofstream logger("logfile.txt", std::ios::app);

#define printLog(...) log(logger,"Message::Fille [", __FILE__, "] fuction[", __FUNCTION__, "] line[", __LINE__, "] message:[", __VA_ARGS__, "]")
#define printError(...) log(logger,"Error::Fille [", __FILE__, "] fuction[", __FUNCTION__, "] line[", __LINE__, "] message:[", __VA_ARGS__, "]")
#define printWarning(...) log(logger,"Warning::Fille [", __FILE__, "] fuction[", __FUNCTION__, "] line[", __LINE__, "] message:[", __VA_ARGS__, "]")

#else
#define printLog(...) 
#define printWarning(...)
#define printError(...)
#endif

#endif // SIMPLE_FTP_LOGGING_HEADER_
