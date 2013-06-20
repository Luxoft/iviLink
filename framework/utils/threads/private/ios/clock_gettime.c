
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */

#ifdef __APPLE__
#include <sys/time.h>

int clock_gettime(int clockType, struct timespec * t)
{
    struct timeval now;
    int retval = gettimeofday(&now, NULL);
    if (retval)
    {
        return retval;
    }
    t->tv_sec = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#endif
