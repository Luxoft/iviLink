
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 12.04.2013
 */
#include <cassert>
#include "CSignalSemaphore.hpp"

SignalSemaphore::SignalSemaphore(int count)
{
    mSemaphore = dispatch_semaphore_create(count);
}

SignalSemaphore::~SignalSemaphore()
{
    dispatch_release(mSemaphore);
}

void SignalSemaphore::wait()
{
    dispatch_semaphore_wait(mSemaphore, DISPATCH_TIME_FOREVER);
}

int SignalSemaphore::tryWait()
{
    return dispatch_semaphore_wait(mSemaphore, DISPATCH_TIME_NOW);
}

int SignalSemaphore::waitTimeout(const time_t miliSec)
{
    dispatch_time_t waitTime = dispatch_time(DISPATCH_TIME_NOW, miliSec * NSEC_PER_MSEC);
    return dispatch_semaphore_wait(mSemaphore, waitTime);
}

void SignalSemaphore::signal()
{
    dispatch_semaphore_signal(mSemaphore);
}