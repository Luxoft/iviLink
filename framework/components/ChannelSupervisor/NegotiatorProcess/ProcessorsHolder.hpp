/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Dispatches messages from clients and the remote negotiator 
 * to INotificationProcessors.
 */

#ifndef __PROCESSORS_HOLDER_HPP
#define __PROCESSORS_HOLDER_HPP

#include <set>

#include "NegotiatorTube.hpp"
#include "Processor.hpp"
#include "Notification.hpp"
#include "Request.hpp"
#include "CMutex.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
typedef std::set<Processor*> tProcessorsSet;
class ProcessorsHolder
{
    ProcessorsHolder();
    ~ProcessorsHolder();
    static ProcessorsHolder * mInstance;
    static CMutex mSingletonMutex;
public:
    static ProcessorsHolder * getInstance();
    static void deleteInstance();
    void setRole(bool isMaster);
    void processTubeNotification(Messages::Notification*, NegotiatorTube*);
    void processClientRequest(Messages::Request*, NegotiatorTube*, IResponseSender*,
                    ResponseContainer);

    void processLocalClientDisconnected(Int32 clientID);
    void processRemoteClientDisconnected(Int32 clientID);

    void invalidateLocalTimedOutRequest(Int32 clientID, UInt32 reqID);
    void invalidateRemoteTimedOutRequest(Int32 clientID, UInt32 reqID);
private:
    tProcessorsSet mProcessors;
    CMutex mProcessorsMutex;
    bool mIsMaster;
    Processor * obtainProcessorForRequest(Messages::Request*);
    Processor * obtainProcessorForNotification(Messages::Notification*);

    static Messages::Notification::NotificationType requestTypeToNotification(
                    Messages::Request::RequestType);
    Processor * createProcessorForType(
                    Messages::Notification::NotificationType);

    static Logger mLogger;
};

}
}

#endif //__PROCESSORS_HOLDER_HPP
