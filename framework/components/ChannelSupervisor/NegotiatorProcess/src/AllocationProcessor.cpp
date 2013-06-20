/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Processes notifications initial allocation messages.
 */
#include <cassert>

#include "AllocationProcessor.hpp"
#include "Map.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

Logger AllocationProcessor::mLogger =
        Logger::getInstance("ChannelSupervisor.NegotiatorProcess.Processors.AllocationProcessor");

AllocationProcessor::AllocationProcessor(bool isMaster)
    : Processor()
    , mGotAllocateRequest(false)
    , mIsMaster(isMaster)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRequestType = Request::REQUESTTYPE_ALLOCATE_CHANNEL;
    mNotificationType = Notification::NOTIFICATIONTTYPE_ALLOCATE;
}

AllocationProcessor::~AllocationProcessor()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void AllocationProcessor::onRemoteNotification(Messages::Notification* notification,
                NegotiatorTube* tube)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Processor::onRemoteNotification(notification, tube);
    if (notification->getType() == Notification::NOTIFICATIONTTYPE_ALLOCATE)
    {
        LOG4CPLUS_INFO(mLogger, "Notification::NOTIFICATIONTTYPE_ALLOCATE");
        mGotAllocateRequest = true;
        if (!mIsMaster)
        {
            mChannelID = notification->getChannelID();
            LOG4CPLUS_INFO(mLogger, "is slave, remote channel id: " 
                + convertIntegerToString(mChannelID));
        }
        if (mRequestType != Request::REQUESTTYPE_UNKNOWN)
        {
            LOG4CPLUS_INFO(mLogger, "Haven't got local allocate request yet");
            mTag = Notification::getComplementingTag(notification->getTag());
        }
        else
        {
            LOG4CPLUS_INFO(mLogger, "already have local allocate request");
            sendNotification(Notification::NOTIFICATIONTTYPE_CHECK_MAP);
            mNotificationType = Notification::NOTIFICATIONTTYPE_CHECK_MAP;
        }
    }
    else if (notification->getType() == Notification::NOTIFICATIONTTYPE_CHECK_MAP)
    {
        LOG4CPLUS_INFO(mLogger, "Notification::NOTIFICATIONTTYPE_CHECK_MAP");
        ErrorCode errc;
        const char * errStr;
        notification->getError(errc, errStr);
        assert(errc == ERRORCODE_SUCCESS);
        LOG4CPLUS_INFO(mLogger, "sending response to client");
        sendResponse(Response::RESPONSETYPE_ALLOCATE_CHANNEL);

        mNotificationType = Notification::NOTIFICATIONTTYPE_UNKNOWN;
    }
    else
    {
        assert(false);
    }
}

void AllocationProcessor::onLocalRequest(Messages::Request* request,
                NegotiatorTube* tube, IResponseSender* responseSender,
                ResponseContainer container)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Processor::onLocalRequest(request, tube, responseSender, container);
    // will not process requests anymore
    mRequestType = Request::REQUESTTYPE_UNKNOWN;

    mTag = request->getTag();
    if (mIsMaster)
    {
        mChannelID = Map::getInstance()->getNextFreeChannelId(request->getChannelID());
        LOG4CPLUS_INFO(mLogger, "is master, generated channel id: " + convertIntegerToString(mChannelID));
    }
    sendNotification(Notification::NOTIFICATIONTTYPE_ALLOCATE);
    if (mGotAllocateRequest)
    {
        LOG4CPLUS_INFO(mLogger, "remote notification came earlier than local ipc request");
        sendNotification(Notification::NOTIFICATIONTTYPE_CHECK_MAP);
        mNotificationType = Notification::NOTIFICATIONTTYPE_CHECK_MAP;
    }
}

bool AllocationProcessor::isFinished() const
{
    return mNotificationType == Notification::NOTIFICATIONTTYPE_UNKNOWN;
}