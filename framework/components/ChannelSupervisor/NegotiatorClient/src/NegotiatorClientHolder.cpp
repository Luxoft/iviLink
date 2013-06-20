/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Holder of singleton instance of the Negotiator Client
 */

#include "NegotiatorClientHolder.hpp"
#include "NegotiatorClient.hpp"
#include <tr1/memory>

using namespace iviLink::ChannelSupervisor;

class iviLink::ChannelSupervisor::NegotiatorClientDeleter
{
public:
    void operator()(NegotiatorClient * p)
    {
        delete p;
    }
};

typedef std::tr1::shared_ptr<NegotiatorClient> tNegotiatorClientPtr;
static tNegotiatorClientPtr instance;
static CMutex singletonMutex;

INegotiatorClient * NegotiatorClientHolder::getInstance()
{
    MutexLocker lock(singletonMutex);
    if (instance.get() == NULL)
    {
        instance = tNegotiatorClientPtr(new NegotiatorClient(), NegotiatorClientDeleter());
    }
    return instance.get();
}