#ifndef __KICK_WD_THREAD_HPP
#define __KICK_WD_THREAD_HPP

#include "CThread.hpp"
#include "Logger.hpp"
#include "CMutex.hpp"

namespace iviLink
{

namespace Channel
{

class KickWatchdogThread : public CThread
{
    KickWatchdogThread();
    virtual ~KickWatchdogThread() {}
public:
    void startKicking();
    static KickWatchdogThread * getInstance();
    static void deleteInstance();

private:
    virtual void threadFunc();
private:
    bool mIsStarted;
    CMutex mIsStartedMutex;

    UInt32 mSleepTimeoutParts; // will sleep KICK_TIMEOUT_MS between kicks, but split in portions

    static KickWatchdogThread * mInstance;
    static CMutex mSingletonMutex;
    static Logger mLogger;
};

}

}

#endif // __KICK_WD_THREAD_HPP