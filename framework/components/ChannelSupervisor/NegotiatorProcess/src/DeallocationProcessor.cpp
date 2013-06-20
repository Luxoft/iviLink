/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Processes deallocation notifications.
 */

#include <cassert>

#include "DeallocationProcessor.hpp"
#include "Map.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

Logger DeallocationProcessor::mLogger 
        = Logger::getInstance("ChannelSupervisor.NegotiatorProcess.Processors.DeallocationProcessor");

DeallocationProcessor::DeallocationProcessor()
    : Processor()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRequestType = Request::REQUESTTYPE_DEALLOCATE_CHANNEL;
    // never receives remote notifications
    mNotificationType = Notification::NOTIFICATIONTTYPE_UNKNOWN;
}

DeallocationProcessor::~DeallocationProcessor()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void DeallocationProcessor::onLocalRequest(Messages::Request* request,
                NegotiatorTube* tube, IResponseSender* responseSender,
                ResponseContainer container)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Processor::onLocalRequest(request, tube, responseSender, container);
    mRequestType = Request::REQUESTTYPE_UNKNOWN;

    mTag = request->getTag();
    mChannelID = request->getChannelID();

    LOG4CPLUS_INFO(mLogger, "invalidating: " + mTag + " id: " 
        + convertIntegerToString(mChannelID));

    Map::getInstance()->invalidateChannelInfo(mChannelID);
    // response is not sent
}

bool DeallocationProcessor::isFinished() const
{
    return mRequestType == Request::REQUESTTYPE_UNKNOWN;
}