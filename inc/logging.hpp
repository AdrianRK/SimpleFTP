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


#ifndef __LOGGING_HEADER__
#define __LOGGING_HEADER__
#include <iostream>

template <typename T>
void log(const T & obj)
{
	std::cout << obj << "\n";
}

template <typename T, typename... agcs>
void log (const T& obj, const agcs&... arg)
{
	std::cout << obj;
	log(arg...);
}

#ifdef D_DEBUG

#define printLog(...) log("fuction[", __FUNCTION__, "] line[", __LINE__, "] message:[", __VA_ARGS__, "]")
#else
#define printLog(...) 
#endif

#endif // __LOGGING_HEADER__
