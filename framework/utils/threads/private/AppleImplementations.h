
/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 21.03.2013
 */

#ifndef iviLinkFwDraft_ApplieImplementations_h
#define iviLinkFwDraft_ApplieImplementations_h
#ifdef __APPLE__

#define CLOCK_REALTIME 1
#define CLOCK_MONOTONIC 1

#include <pthread.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

int pthread_mutex_timedlock (pthread_mutex_t *mutex, struct timespec *timeout);

int clock_gettime(int, struct timespec * t);

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // __APPLE__
#endif // iviLinkFwDraft_ApplieImplementations_h
