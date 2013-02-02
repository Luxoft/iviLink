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


#include <cassert>

#include "SystemControllerMsgProxy.hpp"

using namespace iviLink::Ipc;

namespace AppManMsgProtocol
{

SystemControllerMsgProxy::SystemControllerMsgProxy(const string connectionName)
        : mpIpc(NULL)
{
    mpIpc = new CIpc(connectionName, *this);
}

SystemControllerMsgProxy::~SystemControllerMsgProxy()
{
    delete mpIpc;
}

BaseError SystemControllerMsgProxy::connect()
{
    return mpIpc->connect();
}

bool SystemControllerMsgProxy::isConnected() const
{
    return mpIpc->isConnected();
}

BaseError SystemControllerMsgProxy::requestConnected()
{
    if (!mpIpc)
        return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

    Message* req = reinterpret_cast<Message*>(mWriteBuffer);
    req->header.type = AM_SC_CONNECTION_ESTABLISHED;
    req->header.size = 0;

    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(Message) + req->header.size;
    UInt32 respSize = 0;

    BaseError err = mpIpc->request(id, mWriteBuffer, reqSize, mReadBuffer, respSize);

    if (!err.isNoError())
        return err;

    return BaseError::NoError(getName());
}

bool SystemControllerMsgProxy::isLinkAlive()
{
    bool r = false;
    return getLinkState(r).isNoError() ? r : false;
}

BaseError SystemControllerMsgProxy::getLinkState(bool& is_link_up)
{
    if (!mpIpc)
        return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

    Message msg;
    msg.header.type = AM_SC_GET_LINK_STATE;
    msg.header.size = 0;

    UInt32 resp_sz;

    const BaseError e = mpIpc->request(mMsgIdGen.getNext(), reinterpret_cast<const UInt8*>(&msg),
            sizeof(msg), reinterpret_cast<UInt8*>(&is_link_up), resp_sz);

    assert(!e.isNoError() || resp_sz == 1);

    return e.isNoError() ? BaseError::NoError(getName()) : e;
}

BaseError SystemControllerMsgProxy::requestDisconnected()
{
    if (!mpIpc)
        return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

    Message* req = reinterpret_cast<Message*>(mWriteBuffer);
    req->header.type = AM_SC_CONNECTION_LOST;
    req->header.size = 0;

    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(Message) + req->header.size;
    UInt32 respSize = 0;

    BaseError err = mpIpc->request(id, mWriteBuffer, reqSize, mReadBuffer, respSize);

    if (!err.isNoError())
        return err;

    return BaseError::NoError(getName());
}

void SystemControllerMsgProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
        UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
        iviLink::Ipc::DirectionID)
{
    // this should never happen
}

void SystemControllerMsgProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
        UInt32 payloadSize, iviLink::Ipc::DirectionID)
{
    Message const* req = reinterpret_cast<Message const*>(pPayload);

    switch (req->header.type)
    {
    case SC_AM_SHUTDOWN:
        onShutDown();
        break;
    case SC_AM_LINK_UP_NOTIFY:
        onLinkUpNotification();
        break;
    case SC_AM_LINK_DOWN_NOTIFY:
        onLinkDownNotification();
        break;
    default:
        break;
    }
}

void SystemControllerMsgProxy::OnConnection(iviLink::Ipc::DirectionID)
{

}

void SystemControllerMsgProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{

}

}
