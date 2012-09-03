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










/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
 /********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CChannelAllocator.hpp"
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CSourceAgent.hpp"
#include "CTargetAgent.hpp"
#include "L1InterfaceStub.hpp"
#include "utils/misc/Logger.hpp"
#include "framework/components/ConnectivityAgent/generic/L0/L0Interface.hpp"

using namespace iviLink::ConnectivityAgent::L1;

CMutex CChannelAllocator::mSingletonMutex;
CChannelAllocator* CChannelAllocator::mSelf = 0;


using iviLink::ConnectivityAgent::L0::L0Interface;

Logger CChannelAllocator::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L1.CChannelAllocator"));

CChannelAllocator::CChannelAllocator():
mpL0Interface(0)
{}

CChannelAllocator* CChannelAllocator::getInstance()
{
   mSingletonMutex.lock();
   if (mSelf == NULL)
   {
      mSelf = new CChannelAllocator();
   }
   mSingletonMutex.unlock();
   return mSelf;
}

void CChannelAllocator::deleteInstance()
{
   delete this;
   mSelf = NULL;
}

void CChannelAllocator::setL0Interface(L0Interface *pL0Interface)
{
   mpL0Interface = pL0Interface;
}

void CChannelAllocator::OnConnected()
{
   L1InterfaceStub::getInstance()->OnConnected();
}

void CChannelAllocator::OnDisconnected()
{
   LOG4CPLUS_TRACE(logger, "CChannelAllocator::OnDisconnected()");
   L1InterfaceStub::getInstance()->OnDisconnected();
}

ERROR_CODE CChannelAllocator::allocateChannel(TChannelPriority prio, UInt32 channel_id, L1InterfaceStub::tChannelInfo& channelInfo)
{
   LOG4CPLUS_TRACE(logger, "CChannelAllocator::allocateOutgoingChannel()");

   ERROR_CODE ret = ERR_FAIL;

   /// @todo correct processing of error code and return values. PIlin, 31.08.12
   LOG4CPLUS_WARN(logger, "@todo correct processing of error code and return values. PIlin, 31.08.12");

   if (mpL0Interface)
   {
      iviLink::ConnectivityAgent::L1::CSourceAgent* pSourceAgent = new CSourceAgent(channel_id);

      mpL0Interface->openTransmissionChannel(channel_id, prio, *pSourceAgent,
            channelInfo.mUpperThresholdTime, channelInfo.mLowerThresholdTime);

      channelInfo.mpSourceAgent = pSourceAgent;

      iviLink::ConnectivityAgent::L1::CTargetAgent* pTargetAgent = new CTargetAgent(channel_id);

      channelInfo.mpTargetAgent = pTargetAgent;

      ret = mpL0Interface->openIncomingChannel(channel_id, *pTargetAgent);
      ret = ERR_OK;
   }
   return ret;
}

ERROR_CODE CChannelAllocator::deallocateChannel(UInt32 channel_id)
{
   ERROR_CODE ret = ERR_FAIL;

   if(mpL0Interface)
   {
      mpL0Interface->closeTransmissionChannel(channel_id);
      mpL0Interface->closeIncomingChannel(channel_id);
      ret =  ERR_OK;
   }
   return ret;
}


UInt64 CChannelAllocator::getQoSReportForChannel(const UInt32 channel_id)
{
   return mpL0Interface->getQoSReport(channel_id);
}

void CChannelAllocator::narrowBandwidth(UInt32 channel_id)
{
}

void CChannelAllocator::higherBandwidth(UInt32 channel_id)
{
}

CChannelAllocator::~CChannelAllocator()
{
   mpL0Interface = 0;

}
