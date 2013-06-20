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


#ifndef APPSTATUSAGGREGATOR_HPP
#define APPSTATUSAGGREGATOR_HPP

#include <string>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <queue>
#include <utility>

#include "ICallbackHandler.hpp"
#include "CommonMessage.hpp"
#include "ipc_common.hpp"
#include "CIpc.hpp"
#include "ISender.hpp"
#include "SenderThread.hpp"
#include "AppStatusAggregatorProtocol.hpp"


#include "Logger.hpp"

typedef std::pair <AppStatusAggToApps,std::string> messageInfo;
typedef std::pair<iviLink::Ipc::DirectionID, messageInfo> messageDir;
typedef std::queue<messageDir>  tReqQueue;

namespace iviLink
{
namespace SystemController
{

class AppStatusAggregator : public iviLink::Ipc::ICallbackHandler
                            ,public ISender
{

public:

    AppStatusAggregator();
    virtual ~AppStatusAggregator();

private:

    void sendMessage(messageInfo msgInfo, iviLink::Ipc::DirectionID dirID);
    void startIpcServer() const;
    void pushMsgToQueue(iviLink::Ipc::DirectionID dirId, AppStatusAggToApps type,std::string msg);

    // from ICallbackHandler
    virtual void OnConnection(iviLink::Ipc::DirectionID);
    virtual void OnConnectionLost(iviLink::Ipc::DirectionID);
    virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID dirId);
    virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID dirId);

    //from ISender
    virtual void senderLoop();

    bool hasRequests();
    void handleRequest();

    bool                        mIsConnected;
    iviLink::Ipc::CIpc*         mIpc;
    std::string                 mConnectionName;
    CMsgIdGen                   mMsgIdGen;
    iviLink::Ipc::DirectionID   mLauncherID;
    iviLink::Ipc::DirectionID   mMediaID;
    iviLink::Ipc::DirectionID   mClimateID;

    SenderThread *             mpSenderThread;
    CMutex *                    mpReqMutex;
    CSignalSemaphore *          mpReqSemaphore;
    bool                        mBe;
    tReqQueue                   mReqQueue;
    
    static Logger mLogger;

};

}
}
#endif /* APPSTATUSAGGREGATOR_HPP */
