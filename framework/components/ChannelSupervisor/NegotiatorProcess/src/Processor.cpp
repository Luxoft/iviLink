#include "Processor.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;


Logger Processor::mLogger =
        Logger::getInstance("ChannelSupervisor.NegotiatorProcess.Processors.BaseProcessor");

Processor::Processor()
    : mRequestType(Request::REQUESTTYPE_UNKNOWN)
    , mNotificationType(Notification::NOTIFICATIONTTYPE_UNKNOWN)
    , mResponseSender(NULL)
    , mTube(NULL)
    , mContainer()
    , mChannelID(0)
    , mTag("")
    , mLocalClientID(0)
    , mRemoteClientID(0)
    , mLocalRequestID(0)
    , mRemoteRequestID(0)
{
}

void Processor::onRemoteNotification(Messages::Notification* noti, NegotiatorTube* tube)
{
    mTube = tube;
    mRemoteClientID = noti->getClientID();
    mRemoteRequestID = noti->getRequestID();
}

void Processor::onLocalRequest(Messages::Request*, NegotiatorTube* tube, IResponseSender* sender,
                    ResponseContainer container)
{
    mTube = tube;
    mResponseSender = sender;
    mContainer = container;
    mLocalClientID = container.mClientID;
    mLocalRequestID = container.mResponseID;
}

std::string Processor::getTag() const
{
    return mTag;
}

UInt32 Processor::getChannelID() const
{
    return mChannelID;
}

Notification::NotificationType Processor::getNotificationType() const
{
    return mNotificationType;
}

Messages::Request::RequestType Processor::getRequestType() const
{
    return mRequestType;
}

Int32 Processor::getLocalClientID() const
{
    LOG4CPLUS_INFO(mLogger, "local cli id: " + convertIntegerToString(mLocalClientID));
    return mLocalClientID;
}

Int32 Processor::getRemoteClientID() const
{
    LOG4CPLUS_INFO(mLogger, "remote cli id: " + convertIntegerToString(mRemoteClientID));
    return mRemoteClientID;
}

void Processor::sendResponse(Response::ResponseType type, ErrorCode eCode, const char * errStr)
{
    if (mResponseSender)
    {
        Messages::Response response(mTag.c_str(), mChannelID, type);
        response.setError(eCode, errStr);
        mContainer.mResponseSerialized = response.toString();
        mResponseSender->sendResponse(mContainer);
    }
}

void Processor::sendNotification(Messages::Notification::NotificationType type)
{
    if (mTube)
    {
        Notification noti(mTag.c_str(), mChannelID, mContainer.mClientID, mContainer.mResponseID, type);
        mTube->sendDataTube(noti.toString());
    }
}

UInt32 Processor::getLocalRequestID() const
{
    LOG4CPLUS_INFO(mLogger, "local req id: " + convertIntegerToString(mLocalRequestID));
    return mLocalRequestID;
}

UInt32 Processor::getRemoteRequestID() const
{
    LOG4CPLUS_INFO(mLogger, "remote req id: " + convertIntegerToString(mRemoteRequestID));
    return mRemoteRequestID;
}