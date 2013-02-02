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
#include <cstdlib>
#include <unistd.h>
#include <signal.h>

#include "AuthenticationAppMsgProxy.hpp"
#include "buffer_helpers.hpp"

using namespace iviLink::Ipc;
using namespace iviLink::Ipc::Helpers;

namespace SystemControllerMsgProtocol
{

Logger AuthenticationAppMsgProxy::logger = Logger::getInstance(
        LOG4CPLUS_TEXT("systemController.msgProtocol.AuthenticationAppMsgProxy"));

AuthenticationAppMsgProxy::AuthenticationAppMsgProxy(const string& connectionName)
        : mpIpc(NULL)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    mpIpc = new CIpc(connectionName, *this);
}

AuthenticationAppMsgProxy::~AuthenticationAppMsgProxy()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    delete mpIpc;
}

BaseError AuthenticationAppMsgProxy::requestShutDown()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    if (!mpIpc)
        return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

    AuthenticationMessage* req = reinterpret_cast<AuthenticationMessage*>(mWriteBuffer);
    req->requestType = SC_AA_DIE;
    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();
    UInt32 const reqSize = sizeof(Message);
    return mpIpc->asyncRequest(id, mWriteBuffer, reqSize);
}

BaseError AuthenticationAppMsgProxy::connectAuthenticationApp()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    return mpIpc->beginWaitForConnection();
}

bool AuthenticationAppMsgProxy::isConnected() const
{
    return mpIpc->isConnected();
}

void AuthenticationAppMsgProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
        UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
        iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    LOG4CPLUS_WARN(logger, "Unexpected message");
    bufferSize = 0;
}

void AuthenticationAppMsgProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
        UInt32 payloadSize, iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    AuthenticationMessage const* req = reinterpret_cast<AuthenticationMessage const*>(pPayload);

    switch (req->requestType)
    {
    case AA_SC_AUTHENTICATION_OK:
        LOG4CPLUS_INFO(logger, "AA_SC_AUTHENTICATION_OK");
        onAuthenticationOK();
        break;
    case AA_SC_AUTHENTICATION_NOK:
        LOG4CPLUS_INFO(logger, "AA_SC_AUTHENTICATION_NOK");
        onAuthenticationNOK();
        break;
    case AA_SC_AUTHENTICATION_CANCELED:
        LOG4CPLUS_INFO(logger, "AA_SC_AUTHENTICATION_CANCELED");
        onAuthenticationCanceled();
        break;
    case AA_SC_AUTHENTICATION_ERROR:
        LOG4CPLUS_INFO(logger, "AA_SC_AUTHENTICATION_ERROR");
        onAuthenticationError();
        break;
    default:
        break;
    }

}

void AuthenticationAppMsgProxy::OnConnection(iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    onAuthenticationAppAvailable();
}

void AuthenticationAppMsgProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    onAuthenticationAppNotAvailable();
}

}

