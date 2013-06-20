
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */

#ifdef __APPLE__

#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#include "AppleImplementations.h"

int pthread_mutex_timedlock (pthread_mutex_t *mutex, struct timespec *timeout)
{
    struct timeval timenow;
    struct timespec sleepytime;
    int retcode;
    
    /* This is just to avoid a completely busy wait */
    sleepytime.tv_sec = 0;
    sleepytime.tv_nsec = 10000000; /* 10ms */
    
    while ((retcode = pthread_mutex_trylock (mutex)) == EBUSY)
    {
        gettimeofday (&timenow, NULL);
        if (timenow.tv_sec >= timeout->tv_sec &&
            (timenow.tv_usec * 1000) >= timeout->tv_nsec)
        {
            return ETIMEDOUT;
        }
        nanosleep (&sleepytime, NULL);
    }
    return retcode;
}

#endif