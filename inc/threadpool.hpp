/*
 * =====================================================================================
 *
 *       Filename:  threadpool.hpp
 *
 *    Description:  thread pool
 *
 *        Version:  1.0
 *        Created:  11/08/2018 11:23:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __THREAD_POOL_HEADER__
#define __THREAD_POOL_HEADER__
#include <thread>
#include <queue>
#include <iostream>
#include <condition_variable>
#include <list>
#include <atomic>
#include <unistd.h>



template <typename Job>
class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();
	void postJob(const Job& fobj);

private:
	std::list <std::thread> mListOfThreads;
	std::queue <Job>  mQueue;
	std::mutex mMtx;
	std::atomic<bool> mQuit;
	std::condition_variable_any mSignal;
};

template <typename Job>
ThreadPool<Job>::ThreadPool(): mQuit(false) 
{
	auto lamda = [&]
	{
		while (!this->mQuit)
		{
			bool DoStuff = false;
			
			std::unique_lock <std::mutex> lk(mMtx);
			mSignal.wait(lk, [&] {return (!this->mQueue.empty()) || (this->mQuit);});
			if (this->mQuit)
			{
				break;
			}
			Job local = this->mQueue.front();
			this->mQueue.pop();
			DoStuff = true;
					
			lk.unlock();
			local();	
		}
	};
	for (int i = 0; i < 5; i++)
	{
		mListOfThreads.push_back(std::thread(lamda));
	}
}

template <typename Job>
ThreadPool<Job>::~ThreadPool()
{
	mQuit = true;
	mSignal.notify_all();
	for (std::list<std::thread>::iterator it = mListOfThreads.begin(); it != mListOfThreads.end(); ++it)
	{
		if(it->joinable())
		{
			it->join();
		}
	}
}

template <typename Job>
void ThreadPool<Job>::postJob(const Job& fobj)
{
	std::unique_lock<std::mutex> lk (mMtx);
	mQueue.push(fobj);
	mSignal.notify_all();	
}
#endif //__THREAD_POOL_HEADER__
