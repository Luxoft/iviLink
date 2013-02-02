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
#include "SystemControllerProxy.hpp"

using namespace iviLink::Ipc;
namespace SystemControllerMsgProtocol 
{

Logger SystemControllerProxy::mLogger = Logger::getInstance("systemController.msgProtocol.SystemControllerProxy");

SystemControllerProxy::SystemControllerProxy(const string connectionName)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mIpc = new CIpc(connectionName, *this);
}

SystemControllerProxy::~SystemControllerProxy()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    delete mIpc;
}

void SystemControllerProxy::OnConnection(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void SystemControllerProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   onSystemControllerNotAvaliable();
}

void SystemControllerProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(mLogger, "This shouldn't happen");
   bufferSize = 0;
}

void SystemControllerProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
            iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   SystemControllerToWatchdog message;
   memcpy(&message, pPayload, payloadSize);
   switch (message)
   {
      case SC_WD_LAUNCHED_PHYSICAL_LAYER:
        onPhysicalLayerStarted();
        break;
      case SC_WD_LAUNCHED_CHANNEL_LAYER:
        onChannelLayerStarted();
        break;
      case SC_WD_LAUNCHED_PROFILE_LAYER:
        onProfileLayerStarted();
        break;
      case SC_WD_LAUNCHED_APPLICATION_LAYER:
        onApplicationLayerStarted();
        break;
      case SC_WD_LAUNCHED_AUTHENTICATON:
        onAuthenticationStarted();
        break;
      case SC_WD_STOPPED_AUTHENTICATION:
        onAuthenticationStopped();
        break;
      default:
        LOG4CPLUS_WARN(mLogger, "Unknown input");
        assert(false);
   }
}

BaseError SystemControllerProxy::connectSystemController()
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   return mIpc->connect();
}

bool SystemControllerProxy::isConnected() const
{
    return mIpc->isConnected();
}

}

