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




/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
 #include <unistd.h>

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "../include/CIncomeDispatcher.hpp"
#include "../include/CQoS.hpp"
#include "../include/CTransmitDispatcher.hpp"

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "../include/HALInterface.hpp"

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

Logger HALInterface::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.HALInterface"));

HALInterface::HALInterface(CTransmitDispatcher *pTransmitDispatcher, CIncomeDispatcher* pIncomeDispatcher, CQoS* pQoS) :
   mpTransmitDispatcher(pTransmitDispatcher), 
   mpIncomeDispatcher(pIncomeDispatcher), 
   mpQoS(pQoS)
{
}

HALInterface::~HALInterface()
{
   mpTransmitDispatcher = 0; 
   mpIncomeDispatcher = 0;
   mpQoS = 0;
}

ERROR_CODE HALInterface::openTransmissionChannel(const UInt32 channel_id, const UInt8 priority,
      IBufferProducer& bufferProducer, UInt64 upper_threshold, UInt64 lower_threshold)
{
   LOG4CPLUS_INFO(logger, "HALInterface::openTransmissionChannel(chID = "
         + convertIntegerToString(channel_id) + ", prio = "
         + convertIntegerToString(priority) + ", bufProd = "
         + convertIntegerToString((intptr_t)&bufferProducer) + ", up_tres = "
         + convertIntegerToString(upper_threshold) + ", lo_tres = "
         + convertIntegerToString(lower_threshold));

   if (mpTransmitDispatcher)
   {
      mpTransmitDispatcher->openChannel(channel_id, static_cast<TChannelPriority> (priority), bufferProducer);
   }
   if (mpQoS)
   {
      mpQoS->openChannel(channel_id, upper_threshold, lower_threshold );
   }
   return ERR_OK;
}

ERROR_CODE HALInterface::openIncomingChannel(const UInt32 channel_id,
      IBufferConsumer& bufferConsumer)
{
   LOG4CPLUS_INFO(logger, "HALInterface::openIncomingChannel(chID = "
         + convertIntegerToString(channel_id) + ", bufCons = "
         + convertIntegerToString((intptr_t)&bufferConsumer));

   if (mpIncomeDispatcher)
   {
      return mpIncomeDispatcher->openChannel(channel_id, bufferConsumer);
   }
   return ERR_FAIL;
}

void HALInterface::closeTransmissionChannel(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "HALInterface::closeTransmissionChannel(chID = " + convertIntegerToString(channel_id) + ")");

   if (mpTransmitDispatcher)
   {
      mpTransmitDispatcher->closeChannel(channel_id);
   }
   if (mpQoS)
   {
      mpQoS->closeChannel(channel_id);
   }
}

void HALInterface::closeIncomingChannel(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "HALInterface::closeIncomingChannel(chID = %u" + convertIntegerToString(channel_id) + ")");
   if (mpIncomeDispatcher)
   {
      mpIncomeDispatcher->closeChannel(channel_id);
   }
}

UInt64 HALInterface::getQoSReport(const UInt32 channel_id)
{
   return mpQoS->getAverageForChannel(channel_id);
}
