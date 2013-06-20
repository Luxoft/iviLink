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

#include "AppStatusAggregator.hpp"
#include "Serialize.hpp"

using namespace iviLink::Ipc;
namespace iviLink
{
namespace SystemController
{

    Logger AppStatusAggregator::mLogger = Logger::getInstance("systemController.AppStatusAggregator");

    AppStatusAggregator::AppStatusAggregator()
    : mConnectionName("AppStatusAggregator")
    , mIsConnected(false)
    , mpSenderThread(new SenderThread(this))
    , mpReqMutex(new CMutex())
    , mpReqSemaphore(new CSignalSemaphore())
    , mBe(true)
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        mIpc = new CIpc(mConnectionName, *this);
        startIpcServer();
        mpSenderThread->start();
    }

    AppStatusAggregator::~AppStatusAggregator()
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        delete mIpc;
    }

    void AppStatusAggregator::startIpcServer() const
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        mIpc->beginWaitForConnection();
    }

    void AppStatusAggregator::sendMessage(messageInfo msgInfo, iviLink::Ipc::DirectionID dirID)
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        iviLink::Ipc::MsgID id = mMsgIdGen.getNext();
        UInt8 typeMsg = static_cast<UInt8>(msgInfo.first);
        UInt32 size = sizeof(typeMsg) + msgInfo.second.size();
        UInt8 buf[size];
        memcpy(buf,&typeMsg,sizeof(typeMsg));
        memcpy(buf + sizeof(typeMsg), msgInfo.second.c_str(), msgInfo.second.size());

        bool err = mIpc->asyncRequest(id, buf, size, &dirID).isNoError();
        LOG4CPLUS_INFO(mLogger, "err? " + convertIntegerToString(err));
    }

    // from ICallbackHandler
    void AppStatusAggregator::OnConnection(iviLink::Ipc::DirectionID dirId)
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        mIsConnected = true;
    }

    void AppStatusAggregator::OnConnectionLost(iviLink::Ipc::DirectionID)
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        mIsConnected = false;
    }

    void AppStatusAggregator::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
                UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID dirId) 
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        LOG4CPLUS_ERROR(mLogger, "This shouldn't happen!");
    }

    void AppStatusAggregator::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
                iviLink::Ipc::DirectionID dirId) 
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        LOG4CPLUS_INFO(mLogger, "payloadSize: " + convertIntegerToString(payloadSize));

        UInt32 pos = 0;
        UInt8 type = 0;
        memcpy(&type,pPayload+pos,sizeof(type));
        pos += sizeof(type);

        char chmsg[payloadSize];
        memcpy(chmsg,pPayload + pos,payloadSize);
        std::string msg(chmsg);

        switch(type)
        {
            case TRACK_FROM_MEDIA:
            {
                LOG4CPLUS_INFO(mLogger, "received msg from media: " + msg);
                pushMsgToQueue(mLauncherID,MEDIA,msg);
                break;
            }
            case INIT_FROM_LAUNCHER:
            {
                LOG4CPLUS_INFO(mLogger, "received msg from launcher: " + msg);
                mLauncherID = dirId;
                break;
            }
            case INIT_FROM_MEDIA:
            {
                LOG4CPLUS_INFO(mLogger, "received msg from media: " + msg);
                mMediaID = dirId;
                break;
            }
            case INIT_FROM_CLIMATE:
            {
                LOG4CPLUS_INFO(mLogger, "received msg from climate: " + msg);
                mClimateID = dirId;
                break;
            }
            case INFO_FROM_CLIMATE:
            {
                LOG4CPLUS_INFO(mLogger, "received msg from climate: " + msg);
                pushMsgToQueue(mLauncherID,CLIMATE,msg);
                break;
            }
            case SHOW_FROM_LAUNCHER:
            {
                LOG4CPLUS_INFO(mLogger, "received msg from launcher: " + msg);
                pushMsgToQueue(mMediaID,LAUNCHER,msg);
                break;
            }
            default:
            {
                LOG4CPLUS_WARN(mLogger, "AppStatusAggregator Unknown request");
                break;
            }
        }
    }

    void AppStatusAggregator::pushMsgToQueue(iviLink::Ipc::DirectionID dirId, AppStatusAggToApps type, std::string msg)
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);  
        messageInfo msgInfo = std::make_pair(type,msg);
        messageDir msgDir = std::make_pair(dirId,msgInfo);
        mpReqMutex->lock();
        mReqQueue.push(msgDir);
        mpReqMutex->unlock();
        mpReqSemaphore->signal(); 
    }


    void AppStatusAggregator::senderLoop() 
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        while (mBe) 
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

    bool AppStatusAggregator::hasRequests() 
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        mpReqMutex->lock();
        bool res = !mReqQueue.empty();
        mpReqMutex->unlock();
        return res;
    }

    void AppStatusAggregator::handleRequest()
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
        mpReqMutex->lock();
        messageDir msg;
        msg = mReqQueue.front();
        mReqQueue.pop();
        mpReqMutex->unlock();

        sendMessage(msg.second, msg.first);
    }

}
}