#include <stdlib.h>

#include "ChannelSupervisorWatchdogThread.hpp"
#include "NegotiatorConstants.hpp"

using namespace iviLink::ChannelSupervisor;

#define CSWATCHDOG_THREAD_NAME "ChannelSupervisorWatchdogThread"

#define WD_MAX_SLEEP_TIMEOUT_MS 50

Logger ChannelSupervisorWatchdogThread::mLogger = 
            Logger::getInstance("ChannelSupervisor.NegotiatorProcess.ChannelSupervisorWatchdog.Thread");

ChannelSupervisorWatchdogThread::ChannelSupervisorWatchdogThread()
:CThread(CSWATCHDOG_THREAD_NAME, true)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    div_t sleepyTimes = div(WD_SLEEP_TIMEOUT_MS, WD_MAX_SLEEP_TIMEOUT_MS);
    mSleepTimeoutParts = sleepyTimes.quot;
}

ChannelSupervisorWatchdogThread::~ChannelSupervisorWatchdogThread()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    CThread::stop();
}

void ChannelSupervisorWatchdogThread::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    ChannelSupervisorWatchdog * watchdog = ChannelSupervisorWatchdog::getInstance();
    while (!getStopFlag())
    {
        watchdog->checkChannels();
        UInt32 sleepCounter = 0;
        while (sleepCounter < mSleepTimeoutParts && !getStopFlag())
        {
            CThread::sleep(WD_MAX_SLEEP_TIMEOUT_MS); // to speed up exiting
            sleepCounter ++;
        }
    }   
    ChannelSupervisorWatchdog::deleteInstance();
}