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
#include <condition_variable>
#include <list>
#include <atomic>

template <typename Job, size_t NT = 0>
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

template <typename Job, size_t NT>
ThreadPool<Job, NT>::ThreadPool(): mQuit(false) 
{
	auto lamda = [&]
	{
		while (!this->mQuit)
		{
			std::unique_lock <std::mutex> lk(mMtx);
			mSignal.wait(lk, [&] {return (!this->mQueue.empty()) || (this->mQuit);});
			if (this->mQuit)
			{
				break;
			}
			Job local = this->mQueue.front();
			this->mQueue.pop();
					
			lk.unlock();
			local();	
		}
	};
	size_t lnumberOfThreads = (NT == 0 ? std::thread::hardware_concurrency() : NT);
	if (0 == lnumberOfThreads)
	{
		lnumberOfThreads = 5;
	}
	for (size_t i = 0; i < lnumberOfThreads; i++)
	{
		mListOfThreads.push_back(std::thread(lamda));
	}
}

template <typename Job, size_t NT>
ThreadPool<Job, NT>::~ThreadPool()
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

template <typename Job, size_t NT>
void ThreadPool<Job, NT>::postJob(const Job& fobj)
{
	std::unique_lock<std::mutex> lk (mMtx);
	mQueue.push(fobj);
	mSignal.notify_all();	
}
#endif //__THREAD_POOL_HEADER__
