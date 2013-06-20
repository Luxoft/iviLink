/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Dispatches messages from clients and the remote negotiator 
 * to Processors.
 */

#include "ProcessorsHolder.hpp"

#include "AllocationProcessor.hpp"
#include "ConAgentAllocationDoneProcessor.hpp"
#include "DeallocationProcessor.hpp"
#include "UpdateChannelProcessor.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

ProcessorsHolder * ProcessorsHolder::mInstance = NULL;
CMutex ProcessorsHolder::mSingletonMutex;

Logger ProcessorsHolder::mLogger = Logger::getInstance(
                "ChannelSupervisor.NegotiatorProcess.Processors.ProcessorsHolder");

ProcessorsHolder::ProcessorsHolder()
: mIsMaster(false)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

ProcessorsHolder::~ProcessorsHolder()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mProcessorsMutex);
    for (tProcessorsSet::iterator iter = mProcessors.begin(); iter != mProcessors.end(); iter++)
    {
        delete (*iter);
    }
    mProcessors.clear();
}

ProcessorsHolder * ProcessorsHolder::getInstance()
{
    MutexLocker lock(mSingletonMutex);
    if (mInstance == NULL)
    {
        mInstance = new ProcessorsHolder();
    }
    return mInstance;
}

void ProcessorsHolder::deleteInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

void ProcessorsHolder::processTubeNotification(Messages::Notification* notification,
                NegotiatorTube* tube)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mProcessorsMutex);
    if (notification->getType() == Notification::NOTIFICATIONTTYPE_UNKNOWN)
    {
        LOG4CPLUS_ERROR(mLogger, "Some unknown remote notification type");
        return;
    }
    Processor * processor = obtainProcessorForNotification(notification);
    if (processor)
    {
        processor->onRemoteNotification(notification, tube);
        if (processor->isFinished())
        {
            mProcessors.erase(processor);
            delete processor;
        }
    }
    delete notification;
}

void ProcessorsHolder::processClientRequest(Messages::Request* request,
                NegotiatorTube* tube, IResponseSender* responseSender,
                ResponseContainer container)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mProcessorsMutex);
    Notification::NotificationType type = requestTypeToNotification(request->getRequestType());
    if (type == Notification::NOTIFICATIONTTYPE_UNKNOWN)
    {
        LOG4CPLUS_ERROR(mLogger, "Some unknown client request type");
        return;
    }
    Processor * processor = obtainProcessorForRequest(request);
    if (processor)
    {
        processor->onLocalRequest(request, tube, responseSender, container);
        if (processor->isFinished())
        {
            mProcessors.erase(processor);
            delete processor;
        }
    }
    delete request;
}

void ProcessorsHolder::processLocalClientDisconnected(Int32 clientID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Local client disconnected: " + convertIntegerToString(clientID));
    MutexLocker lock(mProcessorsMutex);
    tProcessorsSet::iterator iter = mProcessors.begin();
    while(iter != mProcessors.end())
    {
        tProcessorsSet::iterator current = iter++;
        if ((*current)->getLocalClientID() == clientID)
        {
            LOG4CPLUS_INFO(mLogger, "found pending processor for local client, deleting");
            delete *current;
            mProcessors.erase(current);
        }
    }
}

void ProcessorsHolder::invalidateLocalTimedOutRequest(Int32 clientID, UInt32 reqID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (clientID == 0 || reqID == 0)
    {
        LOG4CPLUS_WARN(mLogger, "one of the IDs is NULL");
        return;
    }
    MutexLocker lock(mProcessorsMutex);
    LOG4CPLUS_INFO(mLogger, "mProcessors size: " + convertIntegerToString(mProcessors.size()));
    LOG4CPLUS_INFO(mLogger, "looking for pending processor for local client" 
        + convertIntegerToString(clientID) + " for request " + convertIntegerToString(reqID));
    for (tProcessorsSet::iterator iter = mProcessors.begin(); iter != mProcessors.end(); iter++)
    {
        if ((*iter)->getLocalClientID() == clientID && (*iter)->getLocalRequestID() == reqID)
        {
            LOG4CPLUS_INFO(mLogger, "found pending processor for local " 
                + convertIntegerToString(clientID)
                + " for request " + convertIntegerToString(reqID));
            delete (*iter);
            mProcessors.erase(iter);
            return;
        }
    }
}

void ProcessorsHolder::invalidateRemoteTimedOutRequest(Int32 clientID, UInt32 reqID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (clientID == 0 || reqID == 0)
    {
        LOG4CPLUS_WARN(mLogger, "one of the IDs is NULL");
        return;
    }
    MutexLocker lock(mProcessorsMutex);
    LOG4CPLUS_INFO(mLogger, "mProcessors size: " + convertIntegerToString(mProcessors.size()));
    LOG4CPLUS_INFO(mLogger, "looking for pending processor for remote client" 
        + convertIntegerToString(clientID) + " for request " + convertIntegerToString(reqID));
    for (tProcessorsSet::iterator iter = mProcessors.begin(); iter != mProcessors.end(); iter++)
    {
        if ((*iter)->getRemoteClientID() == clientID && (*iter)->getRemoteRequestID() == reqID)
        {
            LOG4CPLUS_INFO(mLogger, "found pending processor for remote " 
                + convertIntegerToString(clientID)
                + " for request " + convertIntegerToString(reqID));
            delete (*iter);
            mProcessors.erase(iter);
            return;
        }
    }
}

void ProcessorsHolder::processRemoteClientDisconnected(Int32 clientID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Remote client disconnected: " + convertIntegerToString(clientID));
    MutexLocker lock(mProcessorsMutex);
    tProcessorsSet::iterator iter = mProcessors.begin();
    while(iter != mProcessors.end())
    {
        tProcessorsSet::iterator current = iter++;
        if ((*current)->getRemoteClientID() == clientID)
        {
            LOG4CPLUS_INFO(mLogger, "found pending processor for remote client, deleting");
            delete *current;
            mProcessors.erase(current);
        }
    }
}

void ProcessorsHolder::setRole(bool isMaster)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "role: " + convertIntegerToString(isMaster));
    mIsMaster = isMaster;
}

Processor * ProcessorsHolder::obtainProcessorForRequest(Messages::Request* request)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    for (tProcessorsSet::iterator iter = mProcessors.begin(); iter != mProcessors.end(); iter++)
    {
        if ((*iter)->getRequestType() == request->getRequestType() 
            && (*iter)->getTag() == request->getTag())
        {
            if (request->getRequestType() != Request::REQUESTTYPE_ALLOCATE_CHANNEL)
            {
                LOG4CPLUS_INFO(mLogger, "request is not for allocate, comparing id");
                if ((*iter)->getChannelID() == request->getChannelID())
                {
                    return (*iter);
                }
                else
                {
                    LOG4CPLUS_INFO(mLogger, "channel id's don't match for tag " + request->getTag());
                }
            }
            else
            {
                LOG4CPLUS_INFO(mLogger, "request is for allocate, NOT comparing id");
                return *iter;
            }
        }
    }
    Processor * processor = createProcessorForType(
                                    requestTypeToNotification(request->getRequestType()));
    if (processor == NULL)
    {
        LOG4CPLUS_ERROR(mLogger, "Some unknown request type");
        return NULL;
    }
    mProcessors.insert(processor);
    return processor;
}

Processor * ProcessorsHolder::obtainProcessorForNotification(Notification * notification)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    std::string complementingTag = Notification::getComplementingTag(notification->getTag());
    for (tProcessorsSet::iterator iter = mProcessors.begin(); iter != mProcessors.end(); iter++)
    {
        if ((*iter)->getNotificationType() == notification->getType() 
            && (*iter)->getTag() == complementingTag)
        {
            if (notification->getType() != Notification::NOTIFICATIONTTYPE_ALLOCATE)
            {
                LOG4CPLUS_INFO(mLogger, "notification is not for allocate, comparing id");
                if ((*iter)->getChannelID() == notification->getChannelID())
                {
                    return (*iter);
                }
                else
                {
                    LOG4CPLUS_INFO(mLogger, "channel id's don't match for tag " + complementingTag);
                }
            }
            else
            {
                LOG4CPLUS_INFO(mLogger, "notification is for allocate, NOT comparing id");
                return *iter;
            }
        }
    }
    Processor * processor = createProcessorForType(notification->getType());
    if (processor == NULL)
    {
        LOG4CPLUS_ERROR(mLogger, "Some unknown notification type");
        return NULL;
    }
    mProcessors.insert(processor);
    return processor;
}

Notification::NotificationType ProcessorsHolder::requestTypeToNotification(
                Request::RequestType type)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    switch (type)
    {
    case Request::REQUESTTYPE_ALLOCATE_CHANNEL:
        return Notification::NOTIFICATIONTTYPE_ALLOCATE;
    case Request::REQUESTTYPE_CA_ALLOCATION_DONE:
        return Notification::NOTIFICATIONTTYPE_CA_ALLOCATE;
    case Request::REQUESTTYPE_DEALLOCATE_CHANNEL:
        return Notification::NOTIFICATIONTTYPE_DEALLOCATE;
    case Request::REQUESTTYPE_UPDATE_CHANNEL_INFO:
        return Notification::NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO;
    default:
        LOG4CPLUS_WARN(mLogger, "no match for RequestType");
        return Notification::NOTIFICATIONTTYPE_UNKNOWN;
    }
}

Processor * ProcessorsHolder::createProcessorForType(
                Messages::Notification::NotificationType type)
{
    switch (type)
    {
    case Notification::NOTIFICATIONTTYPE_ALLOCATE:
        return new AllocationProcessor(mIsMaster);
    case Notification::NOTIFICATIONTTYPE_DEALLOCATE:
        return new DeallocationProcessor();
    case Notification::NOTIFICATIONTTYPE_CA_ALLOCATE:
        return new ConAgentAllocationDoneProcessor();
    case Notification::NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO:
        return new UpdateChannelProcessor();
    default:
        LOG4CPLUS_WARN(mLogger, "no processor for NotificationType");
        return NULL;
    }
}
