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

#include "AppManMsgProxy.hpp"

#include "CommonMessage.hpp"

using namespace iviLink::Ipc;

namespace SystemControllerMsgProtocol
{

Logger AppManMsgProxy::logger = Logger::getInstance(
        LOG4CPLUS_TEXT("systemController.msgProtocol.AppManMsgProxy"));

AppManMsgProxy::AppManMsgProxy(const string connectionName)
        : mpIpc(NULL)
{
    LOG4CPLUS_TRACE(logger, "AppManMsgProxy(" + connectionName + ")");

    mpIpc = new CIpc(connectionName, *this);
}

AppManMsgProxy::~AppManMsgProxy()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    delete mpIpc;
}

BaseError AppManMsgProxy::connectAppMan()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    return mpIpc->beginWaitForConnection();
}

bool AppManMsgProxy::isConnected() const
{
    return mpIpc->isConnected();
}

BaseError AppManMsgProxy::requestShutDown()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    if (!mpIpc)
        return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

    Message* req = reinterpret_cast<Message*>(mWriteBuffer);
    req->header.type = SC_AM_SHUTDOWN;
    req->header.size = 0;

    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(Message) + req->header.size;

    BaseError err = mpIpc->asyncRequest(id, mWriteBuffer, reqSize);

    if (!err.isNoError())
        return err;

    return BaseError::NoError(getName());
}

BaseError AppManMsgProxy::linkUpNotification()
{
    return send_notify(SC_AM_LINK_UP_NOTIFY);
}

BaseError AppManMsgProxy::linkDownNotification()
{
    return send_notify(SC_AM_LINK_DOWN_NOTIFY);
}

BaseError AppManMsgProxy::send_notify(eSCtoAMMessages notify_code)
{
    if (!mpIpc)
        return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");
    const Message msg =
    {
    { notify_code, 0 } };
    return mpIpc->asyncRequest(mMsgIdGen.getNext(), reinterpret_cast<const UInt8*>(&msg),
            sizeof(msg));
}

void AppManMsgProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
        UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    Message const* req = reinterpret_cast<Message const*>(pPayload);

    assert(req->header.size + sizeof(Message) == payloadSize);
    assert(bufferSize >= sizeof(Message));

    bufferSize = 0;
    switch (req->header.type)
    {
    case AM_SC_CONNECTION_ESTABLISHED:
        onCounterAMConnected();
        break;
    case AM_SC_CONNECTION_LOST:
        onCounterAMDisconnected();
    case AM_SC_GET_LINK_STATE:
        *pResponseBuffer = getLinkState();
        bufferSize = sizeof(bool);
        break;
    default:
        break;
    }
}

void AppManMsgProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
        UInt32 payloadSize, iviLink::Ipc::DirectionID)
{

}

void AppManMsgProxy::OnConnection(iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    onAppManAvailable();
}

void AppManMsgProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    onAppManNotAvailable();
}

}
