/* 
 * 
 * iviLINK SDK, version 1.0
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






/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
 /********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "API.hpp"
#include "CConnectivityAgentProxy.hpp"


ERROR_CODE allocateChannel(TChannelPriority prio, UInt32 channel_id, IChannelObserver* observer)
{
   return CConnectivityAgentProxy::getInstance()->allocateChannel(prio,channel_id,observer );
}

ERROR_CODE closeChannel(UInt32 channel_id)
{
   return CConnectivityAgentProxy::getInstance()->deallocateChannel(channel_id);
}

ERROR_CODE sendData(UInt32 channel_id, UInt8 const* data, UInt32 size)
{
   return CConnectivityAgentProxy::getInstance()->sendData(channel_id, data, size);
}

ERROR_CODE receiveData(UInt32 channel_id, UInt8* data, UInt32 & received_size, UInt32 max_size)
{
   return CConnectivityAgentProxy::getInstance()->receiveData(channel_id, data, received_size,max_size);
}

const UInt32 getMaxBufSize()
{
   return MAX_SIZE;
}

ERROR_CODE getFreeSize(UInt32 channel_id, UInt32& free_size)
{
   return CConnectivityAgentProxy::getInstance()->getFreeSize(channel_id, free_size);
}

