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

#include <cassert>

#include "SystemControllerForNegotiator.hpp"

using namespace iviLink::Ipc;

namespace ChannelSupervisorMsgProtocol
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

BaseError SystemControllerMsgProxy::requestConnectionEstablished()
{
    return sendRequest(CS_SC_CONNECTION_ESTABLISHED);
}

BaseError SystemControllerMsgProxy::requestConnectionLost()
{
    return sendRequest(CS_SC_CONNECTION_LOST);
}

BaseError SystemControllerMsgProxy::sendRequest(NegotiatorToSystemController requestType)
{
    if (!mpIpc)
        return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

    Message* req = reinterpret_cast<Message*>(mWriteBuffer);
    req->header.type = requestType;
    req->header.size = 0;

    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(Message) + req->header.size;
    UInt32 respSize = 0;

    return mpIpc->request(id, mWriteBuffer, reqSize, mReadBuffer, respSize);
}

void SystemControllerMsgProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
        UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
        iviLink::Ipc::DirectionID)
{
    bufferSize = 0;
}

void SystemControllerMsgProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
        UInt32 payloadSize, iviLink::Ipc::DirectionID)
{
    switch ((SystemControllerToNegotiator)pPayload[0])
    {
    case SC_CS_SHUTDOWN:
        onShutDown();
        break;
    case SC_CS_ROLE:
        if (payloadSize == 2)
        {
            onReceiveRole((bool)pPayload[1]);
        }
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
