// BSD 2-Clause License
//
// Copyright (c) 2021, Antton Jokinen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef SLVNTHREADPOOL_H
#define SLVNTHREADPOOL_H

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>

namespace slvn_tech
{

class SlvnThread
{
private:
    bool mDestroying;
    std::thread mWorker;
    std::queue<std::function<void()>> mJobQueue;
    std::mutex mQueueMutex;
    std::condition_variable mCondition;

    inline void queueLoop()
    {
        while (true)
        {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(mQueueMutex);
                mCondition.wait(lock, [this] { return !mJobQueue.empty() || mDestroying; });
                if (mDestroying)
                    break;
                job = mJobQueue.front();
            }

            job();

            {
                std::lock_guard<std::mutex> lock(mQueueMutex);
                mJobQueue.pop();
                mCondition.notify_one();
            }
        }
    }

public:
    inline SlvnThread() : mDestroying(false)
    {
        mWorker = std::thread(&SlvnThread::queueLoop, this);
    }

    ~SlvnThread()
	{
		if (mWorker.joinable())
		{
			Wait();
			mQueueMutex.lock();
			mDestroying = true;
			mCondition.notify_one();
	    	mQueueMutex.unlock();
			mWorker.join();
		}
	}

    inline void addJob(std::function<void()> function)
    {
        std::lock_guard<std::mutex> lock(mQueueMutex);
        mJobQueue.push(std::move(function));
        mCondition.notify_one();
    }

    inline void Wait()
    {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        mCondition.wait(lock, [this]() { return mJobQueue.empty(); });
    }
};

class SlvnThreadpool
{
public:
    inline SlvnThreadpool()
    {

    }

public:
    std::vector<std::unique_ptr<SlvnThread>> mThreads;

    inline void SetThreadCount(uint32_t count)
    {
        mThreads.clear();
        for (uint32_t i = 0; i < count; i++)
        {
            mThreads.push_back(std::make_unique<SlvnThread>());
        }
    }

    inline void Wait()
    {
        for (auto& thread : mThreads)
        {
            thread->Wait();
        }
    }
};

} // slvn_tech


#endif // SLVNTHREADPOOL_H