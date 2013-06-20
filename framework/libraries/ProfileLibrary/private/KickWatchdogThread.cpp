#include <stdlib.h>

#include "KickWatchdogThread.hpp"
#include "ChannelSupervisorTube.hpp"
#include "NegotiatorConstants.hpp"

#include "ChannelTagMap.hpp"

#define MAX_SLEEP_MS 50

using namespace iviLink::Channel;

KickWatchdogThread * KickWatchdogThread::mInstance = NULL;
CMutex KickWatchdogThread::mSingletonMutex;
Logger KickWatchdogThread::mLogger = Logger::getInstance("profileLib.Channel.KickCSWatchdogThread");

KickWatchdogThread::KickWatchdogThread()
: CThread("KickWatchdogThread", true), mIsStarted(false)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    div_t divresult = div(iviLink::ChannelSupervisor::KICK_TIMEOUT_MS, MAX_SLEEP_MS);
    mSleepTimeoutParts = divresult.quot;
}

KickWatchdogThread * KickWatchdogThread::getInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mInstance == NULL)
    {
        mInstance = new KickWatchdogThread();
    }
    return mInstance;
}

void KickWatchdogThread::deleteInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mInstance != NULL)
    {
        mInstance->stop();
        delete mInstance;
        mInstance = NULL;
    }
}

void KickWatchdogThread::startKicking()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mIsStartedMutex);
    if (!mIsStarted)
    {
        LOG4CPLUS_INFO(mLogger, "starting thread...");
        mIsStarted = true;
        CThread::start();
    }
}

void KickWatchdogThread::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    while (!getStopFlag())
    {
        int sleepCount = 0;
        while (sleepCount < mSleepTimeoutParts && !getStopFlag())
        {
            CThread::sleep(MAX_SLEEP_MS); // so that it will be stopped faster if needed
            sleepCount ++;
        }
        if (getStopFlag())
        {
            continue;
        }
        tChannelTagMap map;
        ChannelTagMap::getInstance()->getCopyOfMap(map);
        tChannelTagMap::iterator iter;
        for (iter = map.begin(); iter != map.end();  ++iter)
        {
            BaseError err = iviLink::ChannelSupervisor::kickWatchdog(iter->first, iter->second);
            if (!err.isNoError())
            {
                LOG4CPLUS_ERROR(mLogger, "Error: channel " + convertIntegerToString(iter->first) + " wasn't kicked");
            }
            else
            {
                LOG4CPLUS_INFO(mLogger, "kicked channel " + convertIntegerToString(iter->first));
            }
        }
    }
    MutexLocker lock(mIsStartedMutex);
    mIsStarted = false;
}