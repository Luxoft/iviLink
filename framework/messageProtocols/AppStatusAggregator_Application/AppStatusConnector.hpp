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


#ifndef APPSTATUSCONNECTOR_HPP
#define APPSTATUSCONNECTOR_HPP

#include <string>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <queue>
#include <utility>

#include "CIpc.hpp"
#include "CommonMessage.hpp"
#include "ICallbackHandler.hpp"
#include "AppStatusAggregatorProtocol.hpp"
#include "SenderThread.hpp"

#include "Logger.hpp"

typedef std::pair<AppsToAppStatusAgg, std::string>  fullMessage;
typedef std::queue<fullMessage>                     tReqQueue;

class AppStatusConnector : public iviLink::Ipc::ICallbackHandler
                         ,public ISender
{

public:

    AppStatusConnector();
    virtual ~AppStatusConnector();
    virtual void sendMessage(fullMessage message);

protected:

    bool mIsConnected;
    SenderThread *   mpSenderThread;
    CSignalSemaphore *          mpReqSemaphore;
    iviLink::Ipc::DirectionID mDirId;

    void startIpcClient() const;
    void sendMessageViaIPC(const std::string& message, AppsToAppStatusAgg type);

    // from ICallbackHandler
    virtual void OnConnection(iviLink::Ipc::DirectionID);
    virtual void OnConnectionLost(iviLink::Ipc::DirectionID);
    virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID dirId);
    virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID dirId);

    //from ISender
    virtual void senderLoop();
    virtual void connectorLoop();

    bool hasRequests();
    void handleRequest();

private:

    CMutex *                    mpReqMutex;

    bool                        mBe;
    tReqQueue                   mReqQueue;

    iviLink::Ipc::CIpc* mIpc;
    std::string mConnectionName;
    CMsgIdGen mMsgIdGen;
    
    static Logger mLogger;

};

#endif /* APPSTATUSCONNECTOR_HPP */
