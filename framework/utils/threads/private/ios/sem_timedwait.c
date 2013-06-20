
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */

#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "AppleImplementations.h"

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    struct timeval timenow;
    struct timespec sleepytime;
    int retcode;
    
    /* This is just to avoid a completely busy wait */
    sleepytime.tv_sec = 0;
    sleepytime.tv_nsec = 10000000; /* 10ms */
    
    while ((retcode = sem_trywait(sem)) == -1)
    {
        gettimeofday (&timenow, NULL);
        
        if (timenow.tv_sec >= abs_timeout->tv_sec &&
            (timenow.tv_usec * 1000) >= abs_timeout->tv_nsec)
        {
            return -1;
        }
        nanosleep (&sleepytime, NULL);
    }
    
    return 0;
}