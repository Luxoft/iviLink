/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Processes watchdog requests. Works in echo mode on iOS
 */

#include "UpdateChannelProcessor.hpp"

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
    mTag = Notification::getComplementingTag(notification->getTag());
    mChannelID = notification->getChannelID();
    sendNotification(Notification::NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO);
}

void UpdateChannelProcessor::onLocalRequest(Messages::Request* request,
                                            NegotiatorTube* tube, IResponseSender* responseSender,
                                            ResponseContainer container)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Processor::onLocalRequest(request, tube, responseSender, container);
}

void UpdateChannelProcessor::updateMapAndSendResponse()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

bool UpdateChannelProcessor::isFinished() const
{
    return mNotificationType == Notification::NOTIFICATIONTTYPE_UNKNOWN;
}
