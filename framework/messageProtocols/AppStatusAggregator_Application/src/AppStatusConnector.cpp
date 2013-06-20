/* 
 * 
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 * 
 */

#include "AppStatusConnector.hpp"
#include "Serialize.hpp"

using namespace iviLink::Ipc;

Logger AppStatusConnector::mLogger = Logger::getInstance("AppLib.AppStatusConnector");

//public
AppStatusConnector::AppStatusConnector()
: mConnectionName("AppStatusAggregator")
, mpSenderThread(new SenderThread(this))
, mpReqMutex(new CMutex())
, mpReqSemaphore(new CSignalSemaphore())
, mBe(true)
, mIsConnected(false)
, mDirId(0)
{
	LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	mIpc = new CIpc(mConnectionName, *this);
}

AppStatusConnector::~AppStatusConnector()
{
	LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mBe = false;
    delete mIpc;

    mpSenderThread->disconnect();
    delete mpSenderThread;
    delete mpReqMutex;

    while (!mReqQueue.empty()) {
        mReqQueue.pop();
    }
}

//protected
void AppStatusConnector::connectorLoop()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    while(!mIsConnected)
    {
        mIpc->connect();
        sleep(1);
    }
}

void AppStatusConnector::sendMessageViaIPC(const std::string& message, AppsToAppStatusAgg type)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Ipc::MsgID id = mMsgIdGen.getNext(); 
    UInt8 typeMsg = static_cast<UInt8>(type);
    LOG4CPLUS_INFO(mLogger, "message to AppStatusAggregator: " + message); 
    UInt32 size = sizeof(typeMsg) + message.size();
    UInt8 * buf = new UInt8[size];
    memcpy(buf,&typeMsg,sizeof(typeMsg));

    memcpy(buf + sizeof(typeMsg), message.c_str(), message.size());
    mIpc->asyncRequest(id, buf, size);
    
}

void AppStatusConnector::sendMessage(fullMessage message)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mpReqMutex->lock();
    mReqQueue.push(message);
    mpReqMutex->unlock();
    mpReqSemaphore->signal(); 
}

//protected
void AppStatusConnector::startIpcClient() const
{
	LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	mIpc->connect();
}

void AppStatusConnector::senderLoop() 
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    while (mBe)
    {
        connectorLoop();
        while (mIsConnected) 
        {
            if (hasRequests()) 
            {
                handleRequest();
            } 
            else 
            {
                mpReqSemaphore->wait();
            }
        }
    }
}

bool AppStatusConnector::hasRequests() 
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mpReqMutex->lock();
    bool res = !mReqQueue.empty();
    mpReqMutex->unlock();
    return res;
}

void AppStatusConnector::handleRequest()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mpReqMutex->lock();
    fullMessage msg;
    msg = mReqQueue.front();
    mReqQueue.pop();
    mpReqMutex->unlock();

    sendMessageViaIPC(msg.second, msg.first);
}

void AppStatusConnector::OnConnection(iviLink::Ipc::DirectionID dirId)
{
	LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	mIsConnected = true;
    mDirId = dirId;
}

void AppStatusConnector::OnConnectionLost(iviLink::Ipc::DirectionID)
{
	LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mIsConnected = false;
    mpReqSemaphore->signal(); 
}
void AppStatusConnector::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID dirId)
{
	LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}
void AppStatusConnector::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID dirId)
{
	LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}