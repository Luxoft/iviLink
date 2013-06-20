/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Processes watchdog requests.
 */

#include "UpdateChannelProcessor.hpp"

#include <cassert>
#include "Map.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

Logger UpdateChannelProcessor::mLogger = 
        Logger::getInstance("ChannelSupervisor.NegotiatorProcess.Processors.UpdateChannelProcessor");

UpdateChannelProcessor::UpdateChannelProcessor()
    : Processor()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mNotificationType = Notification::NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO;
    mRequestType = Request::REQUESTTYPE_UPDATE_CHANNEL_INFO;
}

UpdateChannelProcessor::~UpdateChannelProcessor()
{
    LOG4CPLUS_INFO(mLogger, __PRETTY_FUNCTION__ + mTag);
}

void UpdateChannelProcessor::onRemoteNotification(Messages::Notification* notification,
                NegotiatorTube* tube)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Processor::onRemoteNotification(notification, tube);
    mNotificationType = Notification::NOTIFICATIONTTYPE_UNKNOWN;
    if (!isFinished())
    {
        LOG4CPLUS_INFO(mLogger, "Didn't get local request yet");
        mTag = Notification::getComplementingTag(notification->getTag());
        mChannelID = notification->getChannelID();
    }
    else
    {
        updateMapAndSendResponse();
    }
}

void UpdateChannelProcessor::onLocalRequest(Messages::Request* request,
                NegotiatorTube* tube, IResponseSender* responseSender,
                ResponseContainer container)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Processor::onLocalRequest(request, tube, responseSender, container);

    mRequestType = Request::REQUESTTYPE_UNKNOWN;

    mTag = request->getTag();
    mChannelID = request->getChannelID();
    sendNotification(Notification::NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO);
    if (isFinished())
    {
        updateMapAndSendResponse();
    }
}

void UpdateChannelProcessor::updateMapAndSendResponse()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "updating map: tag " + mTag 
        + " chID = " + convertIntegerToString(mChannelID));
    ChannelInfo chInfo(mTag, time(NULL));
    Map::getInstance()->updateChannelInfo(mChannelID, chInfo);
    sendResponse(Response::RESPONSETYPE_UPDATE_CHANNEL_INFO);
}

bool UpdateChannelProcessor::isFinished() const
{
    return mNotificationType == Notification::NOTIFICATIONTTYPE_UNKNOWN 
        && mRequestType == Request::REQUESTTYPE_UNKNOWN;
}
