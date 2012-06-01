/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

/***************************************************************************
* Project           AXIS
* (c) copyright     2011
* Company           LUXOFT
* @file             CChannelAllocator.cpp
* @brief            The CChannelAllocator class implementation file
* @author           KSkidanov
***************************************************************************/
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
#include "../../HAL/include/HALInterface.hpp"
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "../include/CChannelAllocator.hpp"
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "../include/CSourceAgent.hpp"
#include "../include/CTargetAgent.hpp"
#include "../include/L0InterfaceStub.hpp"
#include "utils/misc/include/Logger.hpp"

CMutex CChannelAllocator::mSingletonMutex;
CChannelAllocator* CChannelAllocator::mSelf = 0;

Logger CChannelAllocator::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L0.CChannelAllocator"));

CChannelAllocator::CChannelAllocator():
mpHALInterface(0)
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

void CChannelAllocator::setHALInterface(HALInterface *pHALInterface)
{
   mpHALInterface = pHALInterface;
}

void CChannelAllocator::OnConnected()
{
   L0InterfaceStub::getInstance()->OnConnected();
}

void CChannelAllocator::OnDisconnected()
{
   L0InterfaceStub::getInstance()->OnDisconnected();
}

ERROR_CODE CChannelAllocator::allocateChannel(TChannelPriority prio, UInt32 channel_id, L0InterfaceStub::tChannelInfo& channelInfo)
{
   LOG4CPLUS_TRACE(logger, "CChannelAllocator::allocateOutgoingChannel()");

   ERROR_CODE ret = ERR_FAIL;

   if (mpHALInterface)
   {
      CSourceAgent* pSourceAgent = new CSourceAgent(channel_id);

      mpHALInterface->openTransmissionChannel(channel_id, prio, *pSourceAgent,
            channelInfo.mUpperThresholdTime, channelInfo.mLowerThresholdTime);

      channelInfo.mpSourceAgent = pSourceAgent;

      CTargetAgent* pTargetAgent = new CTargetAgent(channel_id);

      channelInfo.mpTargetAgent = pTargetAgent;

      ret = mpHALInterface->openIncomingChannel(channel_id, *pTargetAgent);
      ret = ERR_OK;
   }
   return ret;
}

ERROR_CODE CChannelAllocator::deallocateChannel(UInt32 channel_id)
{
   ERROR_CODE ret = ERR_FAIL;

   if(mpHALInterface)
   {
      mpHALInterface->closeTransmissionChannel(channel_id);
      mpHALInterface->closeIncomingChannel(channel_id);
      ret =  ERR_OK;
   }
   return ret;
}


UInt64 CChannelAllocator::getQoSReportForChannel(const UInt32 channel_id)
{
   return mpHALInterface->getQoSReport(channel_id);
}

void CChannelAllocator::narrowBandwidth(UInt32 channel_id)
{
}

void CChannelAllocator::higherBandwidth(UInt32 channel_id)
{
}

CChannelAllocator::~CChannelAllocator()
{
   mpHALInterface = 0;

}
