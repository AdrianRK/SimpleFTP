/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/23/2018 01:59:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <iostream>
#include "../inc/threadpool.hpp"
#include "../inc/config.hpp"
#include <unistd.h>

class job
{
public:
	virtual ~job() {}
	virtual void operator()() {std::cout << "Do Some Work from " << std::this_thread::get_id() <<"! \n"; sleep(1);}	
};

int main(void)
{
	ThreadPool<job> mPool;

	for(int i = 0; i < 20; ++i)
	{
		mPool.postJob(job());
	}
	
	sleep(10);	

	return 0;
}
