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

#include "ProfileManagerMsgProxy.hpp"
#include "helpers/buffer_helpers.hpp"

using namespace iviLink::Ipc;
using iviLink::Ipc::Helpers::CBufferReader;
using iviLink::Ipc::Helpers::CBufferWriter;

namespace SystemControllerMsgProtocol
{

Logger ProfileManagerMsgProxy::logger = Logger::getInstance(
        LOG4CPLUS_TEXT("systemController.msgProtocol.ProfileManagerMsgProxy"));

ProfileManagerMsgProxy::ProfileManagerMsgProxy(const string connectionName)
        : mpIpc(NULL)
{
    LOG4CPLUS_TRACE(logger, "ProfileManagerMsgProxy(" + connectionName + ")");

    mpIpc = new CIpc(connectionName, *this);
}

ProfileManagerMsgProxy::~ProfileManagerMsgProxy()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    delete mpIpc;
}

BaseError ProfileManagerMsgProxy::connectProfileManager()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    return mpIpc->beginWaitForConnection();
}

bool ProfileManagerMsgProxy::isConnected() const
{
    return mpIpc->isConnected();
}

BaseError ProfileManagerMsgProxy::requestShutDown()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    UInt32 respSize = 0;
    return sendRequest(SC_PM_SHUTDOWN, respSize);
}

BaseError ProfileManagerMsgProxy::requestUnlockAuthenticationProfile()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    return requestUnlock(SC_PM_UNLOCK_AUTHENTICATION_PROFILE);
}

BaseError ProfileManagerMsgProxy::requestUnlockProfiles()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    return requestUnlock(SC_PM_UNLOCK_PROFILES);
}

BaseError ProfileManagerMsgProxy::requestAuthenticationDone()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    UInt32 respSize = 0;
    return sendRequest(SC_PM_AUTHENTICATION_DONE, respSize);
}

BaseError ProfileManagerMsgProxy::sendRequest(SystemControllerToProfileManager requestType,
        UInt32& responseSize)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    if (!mpIpc)
        return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

    Message* req = reinterpret_cast<Message*>(mWriteBuffer);
    req->header.type = requestType;
    req->header.size = 0;

    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(Message) + req->header.size;
    return mpIpc->request(id, mWriteBuffer, reqSize, mReadBuffer, responseSize);
}

BaseError ProfileManagerMsgProxy::requestUnlock(SystemControllerToProfileManager requestType)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    UInt32 respSize = BUFFER_SIZE;
    BaseError err = sendRequest(requestType, respSize);
    if (!err.isNoError())
        return err;

    Message* resp = reinterpret_cast<Message*>(mReadBuffer);
    assert(respSize >= sizeof(Message));
    assert(resp->header.size + sizeof(Message) == respSize);

    bool wasPMError = false;
    CBufferReader reader(resp->data, resp->header.size);

    err = reader.read(wasPMError);
    if (!err.isNoError())
        return err;

    if (wasPMError)
        return BaseError(1, getName(), BaseError::IVILINK_ERROR, "Error unlocking profile");

    return BaseError::NoError(getName());
}

void ProfileManagerMsgProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
        UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
        iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    Message const* req = reinterpret_cast<Message const*>(pPayload);

    assert(req->header.size + sizeof(Message) == payloadSize);
    assert(bufferSize >= sizeof(Message));

    switch (req->header.type)
    {
    case PM_SC_CONNECTION_ESTABLISHED:
        onCounterPMConnected();
        break;
    case PM_SC_CONNECTION_LOST:
        onCounterPMDisconnected();
        break;
    default:
        break;
    }

    bufferSize = 0;
}

void ProfileManagerMsgProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
        UInt32 payloadSize, iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
}

void ProfileManagerMsgProxy::OnConnection(iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    onProfileManagerAvailable();
}

void ProfileManagerMsgProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    onProfileManagerNotAvailable();
}

}
