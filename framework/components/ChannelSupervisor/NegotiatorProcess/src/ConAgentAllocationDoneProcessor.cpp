/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Processes notifications that are sent after the channel has been negotiated and allocated
 * in the Connectivity Agent.
 */

#include <cassert>

#include "ConAgentAllocationDoneProcessor.hpp"
#include "Map.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

Logger ConAgentAllocationDoneProcessor::mLogger = 
        Logger::getInstance("ChannelSupervisor.NegotiatorProcess.Processors.ConAgentAllocationDoneProcessor");

ConAgentAllocationDoneProcessor::ConAgentAllocationDoneProcessor()
    : Processor()
    , mGotRemoteNotificationCaalloc(false)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mNotificationType = Notification::NOTIFICATIONTTYPE_CA_ALLOCATE;
    mRequestType = Request::REQUESTTYPE_CA_ALLOCATION_DONE;
}

ConAgentAllocationDoneProcessor::~ConAgentAllocationDoneProcessor()
{
    LOG4CPLUS_INFO(mLogger, __PRETTY_FUNCTION__ + mTag);
}

void ConAgentAllocationDoneProcessor::onRemoteNotification(
                Messages::Notification* notification, NegotiatorTube* tube)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Processor::onRemoteNotification(notification, tube);
    if (notification->getType() == Notification::NOTIFICATIONTTYPE_CA_ALLOCATE)
    {
        LOG4CPLUS_INFO(mLogger, "NOTIFICATIONTTYPE_CA_ALLOCATE");
        assert(!mGotRemoteNotificationCaalloc);
        mGotRemoteNotificationCaalloc = true;
        if (mRequestType != Request::REQUESTTYPE_UNKNOWN)
        {
            // didn't get local request yet
            mTag = Notification::getComplementingTag(notification->getTag());
            mChannelID = notification->getChannelID();
        }
        else
        {
            assert(mChannelID == notification->getChannelID());
            sendNotification(Notification::NOTIFICATIONTTYPE_MAP_ADD_CID);
            mNotificationType = Notification::NOTIFICATIONTTYPE_MAP_ADD_CID;
        }
    }
    else if (notification->getType() == Notification::NOTIFICATIONTTYPE_MAP_ADD_CID)
    {
        LOG4CPLUS_INFO(mLogger, "NOTIFICATIONTTYPE_MAP_ADD_CID");
        
        ErrorCode errc;
        const char * errStr;
        notification->getError(errc, errStr);
        assert(errc == ERRORCODE_SUCCESS);
        
        // updating map
        ChannelInfo info(mTag, time(NULL));
        Map::getInstance()->addChannelInfo(mChannelID, info);

        sendResponse(Response::RESPONSETYPE_CA_ALLOCATION_DONE);
        mNotificationType = Notification::NOTIFICATIONTTYPE_UNKNOWN;
    }
    else
    {
        assert(false);
    }
}

void ConAgentAllocationDoneProcessor::onLocalRequest(Messages::Request* request,
                NegotiatorTube* tube, IResponseSender* responseSender,
                ResponseContainer container)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Processor::onLocalRequest(request, tube, responseSender, container);

    mRequestType = Request::REQUESTTYPE_UNKNOWN;

    mTag = request->getTag();
    mChannelID = request->getChannelID();
    sendNotification(Notification::NOTIFICATIONTTYPE_CA_ALLOCATE);
    if (mGotRemoteNotificationCaalloc)
    {
        sendNotification(Notification::NOTIFICATIONTTYPE_MAP_ADD_CID);
        mNotificationType = Notification::NOTIFICATIONTTYPE_MAP_ADD_CID;
    }
}

bool ConAgentAllocationDoneProcessor::isFinished() const
{
    return mNotificationType == Notification::NOTIFICATIONTTYPE_UNKNOWN;
}
