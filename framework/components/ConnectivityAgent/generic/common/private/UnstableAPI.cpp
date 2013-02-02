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


#include "CConnectivityAgentProxy.hpp"
#include "UnstableAPI.hpp"
#include "Serialize.hpp"


ConnectionInformation::ConnectionInformation(const char * localAddr, const char * remoteAddr, const char * connectionType)
{
  if(!localAddr)
  {
    strLocalAddr = "n/a";
  } 
  else 
  {
    strLocalAddr = localAddr;
  }

  if(!remoteAddr)
  {
    strRemoteAddr = "n/a";
  }
  else
  {
    strRemoteAddr = remoteAddr;
  }

  if(!connectionType)
  {
    strConnectionType = "n/a";
  }
  else
  {
    strConnectionType = connectionType;
  }
}

std::string ConnectionInformation::getLocalAddress()
{
	return strLocalAddr;
}

std::string ConnectionInformation::getRemoteAddress()
{
   	return strRemoteAddr;
}

std::string ConnectionInformation::getConnectionType()
{
  	return strConnectionType;
}

UInt8 * ConnectionInformation::serialize()
{
  	UInt32 size = getSerializedSize();
   	UInt8 * result = new UInt8[size];
   	int pos = 0;
   	iviLink::stringToBuffer(strConnectionType, result);
   	pos = iviLink::stringInBufSize(strConnectionType);
   	iviLink::stringToBuffer(strLocalAddr, result + pos);
   	pos += iviLink::stringInBufSize(strLocalAddr);
   	iviLink::stringToBuffer(strRemoteAddr, result + pos);
   	return result;
}

void ConnectionInformation::deserialize(UInt8 * data)
{
   	int pos = 0;
    strConnectionType = iviLink::bufferToString(data);
    pos = iviLink::stringInBufSize(strConnectionType);
    strLocalAddr = iviLink::bufferToString(data + pos);
    pos += iviLink::stringInBufSize(strLocalAddr);
    strRemoteAddr = iviLink::bufferToString(data + pos);
}

UInt32 ConnectionInformation::getSerializedSize()
{
  	UInt32 size = iviLink::stringInBufSize(strLocalAddr) 
   				+ iviLink::stringInBufSize(strConnectionType) 
   				+ iviLink::stringInBufSize(strRemoteAddr);
   	return size;
}


iviLink::Error getConnectionInformation(ConnectionInformation* data)
{
	iviLink::ConnectivityAgent::ConnectivityAgentError requestResult 
        = CConnectivityAgentProxy::getInstance()->getConnectionInformation(data);
  if (requestResult.isNoError())
  {
    return iviLink::Error::NoError();
  }
  return iviLink::Error(iviLink::IPC_REQUEST_FAILED, "Could not reach Connectivity Agent, try again later", true);
}

