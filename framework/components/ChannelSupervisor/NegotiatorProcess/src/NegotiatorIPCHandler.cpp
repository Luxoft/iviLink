/**
 * Copyright placeholder
 */
/**
 * @ingroup             Channel Supervisor
 * @author              Elena Bratanova <ebratanova@luxoft.com>
 * @date                23.04.2013
 *
 * Communicates with negotiator clients.
 */

#include "NegotiatorIPCHandler.hpp"
#include "Request.hpp"
#include "ProcessorsHolder.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

Logger NegotiatorIPCHandler::mLogger = Logger::getInstance(
                "ChannelSupervisor.NegotiatorProcess.NegotiatorIPCHandler");

NegotiatorIPCHandler::NegotiatorIPCHandler(std::string const& addr, NegotiatorTube * tube)
    : CThread("NegotiatorIPCHandler", true)
    , mTube(tube)
    , mIpc(addr.c_str(), *this)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    CThread::start();
    mIpc.beginWaitForConnection();
}

NegotiatorIPCHandler::~NegotiatorIPCHandler()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mResponseQueueSema.signal();
    CThread::stop();
}

void NegotiatorIPCHandler::OnConnectionLost(iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Notification notification("", 0, dirId, 0, Notification::NOTIFICATIONTTYPE_CLIENT_DISCONNECTED);
    mTube->sendDataTube(notification.toString());
    ProcessorsHolder::getInstance()->processLocalClientDisconnected(dirId);
}

void NegotiatorIPCHandler::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
                iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    bufferSize = 0;
}

void NegotiatorIPCHandler::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                UInt32 payloadSize, iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    pugi::xml_document* doc = new pugi::xml_document();
    doc->load((char*) pPayload);
    Request * request = new Request(doc);
    ResponseContainer container(dirId, id, "");
    if (request->getRequestType() == Request::REQUESTTYPE_INVALIDATE_TIMEDOUT_REQUEST)
    {
        LOG4CPLUS_INFO(mLogger, "invalidating timed out request!");
        Notification notification("", 0, dirId, 
            id, Notification::NOTIFICATIONTTYPE_INVALIDATE_TIMEDOUT_REQUEST);
        mTube->sendDataTube(notification.toString());
        ProcessorsHolder::getInstance()->invalidateLocalTimedOutRequest(dirId, id);
        delete request;
    }
    else
    {
        // request is deleted inside processClientRequest
        ProcessorsHolder::getInstance()->processClientRequest(request, mTube, this,
            container);
    }
}

void NegotiatorIPCHandler::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    while (!getStopFlag())
    {
        mResponseQueueSema.wait();
        mResponseQueueMutex.lock();
        if (mResponseQueue.empty())
        {
            mResponseQueueMutex.unlock();
            LOG4CPLUS_INFO(mLogger, "mResponseQueue is empty!");
            return;
        }
        else
        {
            ResponseContainer container = mResponseQueue.front();
            mResponseQueue.pop_front();
            mResponseQueueMutex.unlock();
            if (!container.mResponseSerialized.empty())
            {
                std::string response = container.mResponseSerialized;
                LOG4CPLUS_INFO(mLogger, "Sending to client: \n" + response);
                mIpc.asyncRequest(container.mResponseID, (const UInt8*) response.c_str(),
                                response.length(), &container.mClientID);
            }
        }
    }
}

void NegotiatorIPCHandler::sendResponse(ResponseContainer container)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mResponseQueueMutex);
    mResponseQueue.push_back(container);
    mResponseQueueSema.signal();
}
