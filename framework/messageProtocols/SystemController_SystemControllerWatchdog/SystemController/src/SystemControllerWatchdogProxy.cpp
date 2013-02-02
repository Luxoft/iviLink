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
#include <strings.h>

#include "SystemControllerWatchdogProxy.hpp"

using namespace iviLink::Ipc;
namespace SystemControllerMsgProtocol 
{

Logger SystemControllerWatchdogProxy::mLogger = Logger::getInstance("systemController.msgProtocol.SystemControllerWatchdogProxy");

SystemControllerWatchdogProxy::SystemControllerWatchdogProxy(const string connectionName)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mIpc = new CIpc(connectionName, *this);
}

SystemControllerWatchdogProxy::~SystemControllerWatchdogProxy()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    delete mIpc;
}

BaseError SystemControllerWatchdogProxy::onPhysicalLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_PHYSICAL_LAYER);
}

BaseError SystemControllerWatchdogProxy::onChannelLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_CHANNEL_LAYER);
}

BaseError SystemControllerWatchdogProxy::onProfileLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_PROFILE_LAYER);
}

BaseError SystemControllerWatchdogProxy::onApplicationLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_APPLICATION_LAYER);
}

BaseError SystemControllerWatchdogProxy::onAuthenticationStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_LAUNCHED_AUTHENTICATON);    
}

BaseError SystemControllerWatchdogProxy::onAuthenticationStopped()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return sendMessage(SC_WD_STOPPED_AUTHENTICATION);    
}

BaseError SystemControllerWatchdogProxy::sendMessage(SystemControllerToWatchdog message)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    if (!mIpc)
      return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();
    UInt32 const reqSize = sizeof(SystemControllerToWatchdog);
    UInt8 messageArray[reqSize];
    bzero(messageArray, reqSize);
    memcpy(messageArray, &message, reqSize);
    return mIpc->asyncRequest(id, messageArray, reqSize);
}

void SystemControllerWatchdogProxy::OnConnection(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   onSystemControllerWatchdogAvaliable();
}

void SystemControllerWatchdogProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   onSystemControllerWatchdogNotAvaliable();
}

void SystemControllerWatchdogProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(mLogger, "This shouldn't happen");
   bufferSize = 0;
}

void SystemControllerWatchdogProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(mLogger, "This shouldn't happen");
}

BaseError SystemControllerWatchdogProxy::connectSystemControllerWatchdog()
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   return mIpc->beginWaitForConnection();
}

bool SystemControllerWatchdogProxy::isConnected() const
{
    return mIpc->isConnected();
}

}

