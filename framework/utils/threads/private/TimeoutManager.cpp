/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#include "TimeoutManager.hpp"
#include "CMutex.hpp"
#include "CThreadPool.hpp"

#include <sys/time.h>

namespace iviLink
{
namespace Utils
{

TimeoutManager::TimeoutManager(UInt32 numberOfThreads)
    : CThread("TimeoutManager", true)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("utils")))
    , mNumberOfThreads(numberOfThreads)
    , mSubscriberMapMutex(NULL)
    , mThreadSemaphore(NULL)
    , mBe (true)
    , mThreadPool(NULL)
    , mStartSemaphore(NULL)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    internalInit();
}

TimeoutManager::TimeoutManager()
    : CThread("TimeoutManager", true)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("utils")))
    , mNumberOfThreads(1)
    , mSubscriberMapMutex(NULL)
    , mThreadSemaphore(NULL)
    , mBe (true)
    , mThreadPool(NULL)
    , mStartSemaphore(NULL)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    internalInit();
}

TimeoutManager::~TimeoutManager()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    mSubscriberMapMutex->lock();
    for (tSubscribersMap::iterator it = mSubscriberMap.begin();
            mSubscriberMap.end() != it; ++it)
    {
        delete it->second;
    }
    mSubscriberMap.clear();
    mSubscriberMapMutex->unlock();

    delete mStartSemaphore;
    delete mThreadPool;
    delete mThreadSemaphore;
    delete mSubscriberMapMutex;
}

void TimeoutManager::internalInit()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (mNumberOfThreads < 1)
    {
        mNumberOfThreads = 1;
    }
    else if (mNumberOfThreads > TIMEOUT_MANAGER_MAX_NUMBER_OF_THREADS)
    {
        mNumberOfThreads = TIMEOUT_MANAGER_MAX_NUMBER_OF_THREADS;
    }

    mSubscriberMapMutex = new CMutex();
    mThreadSemaphore = new CSignalSemaphore();
    mThreadPool = new CThreadPool(mNumberOfThreads);
    mStartSemaphore = new CSignalSemaphore();

    start();
    mStartSemaphore->wait();
}

void TimeoutManager::addSubscriber(ITimeoutSubscriber * subscriber, UInt32 ms)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    Int64 curTime = getCurMs();
    Int64 timeoutTime = curTime + ms;

    bool needReSetTimer = false;
    mSubscriberMapMutex->lock();

    if (mSubscriberMap.empty() ||
            (timeoutTime < mSubscriberMap.begin()->first && mSubscriberMap.empty()))
    {
        needReSetTimer = true;
    }

    mSubscriberMap.insert(make_pair(timeoutTime, subscriber));
    mSubscriberMapMutex->unlock();

    if (needReSetTimer)
    {
        mThreadSemaphore->signal();
    }
}

void TimeoutManager::removeSubscriber(ITimeoutSubscriber * subscriber)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    bool needReSetTimer = false;
    mSubscriberMapMutex->lock();
    if (mSubscriberMap.empty())
    {
        needReSetTimer = true;
    }
    else
    {
        for (tSubscribersMap::iterator it = mSubscriberMap.begin();
                mSubscriberMap.end() != it; ++it)
        {
            if (subscriber == it->second)
            {
                if (it == mSubscriberMap.begin())
                {
                    needReSetTimer = true;
                }
                mSubscriberMap.erase(it);
                break;
            }
        }
    }
    mSubscriberMapMutex->unlock();

    if (needReSetTimer)
    {
        mThreadSemaphore->signal();
    }
}

Int64 TimeoutManager::getCurMs()
{
    timeval now;
    gettimeofday(&now, NULL);
    Int64 nowi = ((Int64)now.tv_sec) * 1000;
    nowi += (Int64)now.tv_usec/1000;
    return nowi;
}

void TimeoutManager::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mStartSemaphore->signal();

    while (mBe)
    {
        processTimeout();
    smartWait();
    }
}

void TimeoutManager::smartWait()
{
    bool empty = false;
    Int64 timeout = 0;
    mSubscriberMapMutex->lock();
    if ( mSubscriberMap.empty())
    {
        empty = true;
    }
    else
    {
        timeout = mSubscriberMap.begin()->first - getCurMs();
    }
    mSubscriberMapMutex->unlock();
    if (empty)
    {
        mThreadSemaphore->wait();
    }
    else if (timeout > 0)
    {
        mThreadSemaphore->waitTimeout(timeout);
    }
}

void TimeoutManager::processTimeout()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    Int64 now = getCurMs();
    while (processTopTimeout(now));
}

class JobHolder : public IThreadPoolJobData
{
public:
    ITimeoutSubscriber * mSubscriber;
    JobHolder(ITimeoutSubscriber * subscriber)
        : mSubscriber(subscriber)
    {
    }

    virtual ~JobHolder()
    {
        delete mSubscriber;
    }
};

void jobPerformer(IThreadPoolJobData const* job)
{
    const JobHolder * jh = static_cast<const JobHolder*>(job);
    jh->mSubscriber->onTimeout();
}

bool TimeoutManager::processTopTimeout(Int64 nowMs)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    mSubscriberMapMutex->lock();
    LOG4CPLUS_INFO(mLogger, "now: " + convertIntegerToString(nowMs) + ", first: "
            + convertIntegerToString(mSubscriberMap.begin()->first));

    if (mSubscriberMap.empty() || nowMs < mSubscriberMap.begin()->first)
    {
        mSubscriberMapMutex->unlock();
        LOG4CPLUS_INFO(mLogger, "return false");
        return false;
    }

    ITimeoutSubscriber * pSubscriber = mSubscriberMap.begin()->second;
    mSubscriberMap.erase(mSubscriberMap.begin());
    mSubscriberMapMutex->unlock();
    CThreadPoolJob job(jobPerformer,new JobHolder(pSubscriber));
    mThreadPool->addJob(job);
    return true;
}

void TimeoutManager::finish()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    mBe = false;
    mThreadSemaphore->signal();
    stop();
}

}   // namespace Utils
}   // namespace iviLink
