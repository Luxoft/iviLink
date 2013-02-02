/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
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
 */ 


#ifndef APP_MAP_MSG_PROXY_HPP
#define APP_MAN_MSG_PROXY_HPP

#include <string>
#include "BaseError.hpp"
#include "ICallbackHandler.hpp"
#include "ipc_common.hpp"
#include "CIpc.hpp"

#include "Logger.hpp"

#include "SysCtrlAmpProtocol.hpp"
#include "CommonMessage.hpp"

using namespace std;
using namespace iviLink::Ipc;

namespace SystemControllerMsgProtocol
{

class AppManMsgProxy: public iviLink::Ipc::ICallbackHandler
{
private:
    static Logger logger;

    const char* getName()
    {
        return "AppManMsgProxy";
    }

    iviLink::Ipc::CIpc* mpIpc;

public:
    explicit AppManMsgProxy(const string connectionName);
    virtual ~AppManMsgProxy();

protected:

    // Outgoing messages
    //
    BaseError requestShutDown();

    BaseError linkUpNotification();    // SC_AM_LINK_UP_NOTIFY
    BaseError linkDownNotification();  // SC_AM_LINK_DOWN_NOTIFY

    // Incoming messages
    // should be implemented by implementation
    virtual BaseError onCounterAMConnected() = 0;
    virtual BaseError onCounterAMDisconnected() = 0;

protected:
    // from ICallbackHandler

    virtual void OnConnection(iviLink::Ipc::DirectionID);
    virtual void OnConnectionLost(iviLink::Ipc::DirectionID);
    virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID);
    virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID);

    virtual void onAppManAvailable() = 0;
    virtual void onAppManNotAvailable() = 0;

    // should return true if link is up
    virtual bool getLinkState() = 0;  // AM_SC_GET_LINK_STATE

    CMsgIdGen mMsgIdGen;

    BaseError connectAppMan();

    bool isConnected() const;

    UInt8 mReadBuffer[BUFFER_SIZE];
    UInt8 mWriteBuffer[BUFFER_SIZE];

    BaseError send_notify(eSCtoAMMessages notify_code);
};

}
#endif /* APP_MAN_MSG_PROXY_HPP */
