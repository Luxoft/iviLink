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
 * @file             CTransmitDispatcher.cpp
 * @brief            CTransmitDispatcher class implementation file
 * @author           KSkidanov
 ***************************************************************************/

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <assert.h>
#include <time.h>
#include <unistd.h>
/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "../include/CCarrierAdapter.hpp"
#include "../include/CTransmittedFramesQueue.hpp"
#include "../include/CSource.hpp"
#include "../include/Frame.hpp"
#include "../include/IBufferProducer.hpp"
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "../include/CTransmitDispatcher.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "utils/misc/include/Logger.hpp"

Logger CTransmitDispatcher::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CTransmitDispatcher"));

CTransmitDispatcher::CTransmitDispatcher(CCarrierAdapter* pCarrierAdaptor, CTransmittedFramesQueue* pTransmittedFrameQueue) :
   CThread("HALTransmitDispatcher"),
   mpCarrier(pCarrierAdaptor),
   mpTransmittedFrameQueue(pTransmittedFrameQueue),
   mDestroyed(false)
{
   mRegistry.reserve(PRIO_NUM);
   TSourceList list;

   for (int i = 0; i < PRIO_NUM; ++i)
   {
      mRegistry.push_back(list);
   }
   start();
}

const CSource* CTransmitDispatcher::openChannel(const UInt32 channel_id, const TChannelPriority prio, IBufferProducer& bufferProducer)
{
   CSource* pSource = NULL;
   LOG4CPLUS_INFO(logger, "CTransmitDispatcher::openChannel(chID = "
         + convertIntegerToString(channel_id) + ", prio = "
         + convertIntegerToString((int)prio) + ", bufProd = "
         + convertIntegerToString((intptr_t)&bufferProducer) + ")");

   mRegistryMutex.lock();
   TSourceList::iterator iter = mRegistry[static_cast<UInt8> (prio)].begin();
   while (iter != mRegistry[static_cast<UInt8> (prio)].end())
   {
      if (channel_id == static_cast<CSource*> (*iter)->getChannelID())
      {
         break;
      }
      ++iter;
   }
   if (iter == mRegistry[static_cast<UInt8> (prio)].end())
   {
      pSource = new CSource(channel_id);
      LOG4CPLUS_INFO(logger, "CTransmitDispatcher::openChannel() number = "
            + convertIntegerToString(channel_id) + " prio = "
            + convertIntegerToString(static_cast<UInt8>(prio)) + " opening new channel");
      pSource->registerProducer(&bufferProducer);
      bufferProducer.registerConsumer(pSource);
      mRegistry[prio].push_back(pSource);
   }
   mRegistryMutex.unlock();
   return pSource;
}

void CTransmitDispatcher::closeChannel(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "CTransmitDispatcher::closeChannel(channel_id = " +
         convertIntegerToString(channel_id) + ")");
   TSourceList::iterator iter;

   mpTransmittedFrameQueue->deleteFramesForChannel(channel_id);

   mRegistryMutex.lock();

   for (int i = 0; i < PRIO_NUM; ++i)
   {
      iter = mRegistry[i].begin();
      while (iter != mRegistry[i].end())
      {
         if (channel_id == static_cast<CSource*> (*iter)->getChannelID())
         {
            break;
         }
         ++iter;
      }
      if (iter != mRegistry[i].end())
      {
         LOG4CPLUS_INFO(logger, "CTransmitDispatcher::closeChannel(channel_id = "
              + convertIntegerToString(channel_id) + ") deleting CSource");
         delete *iter;
         mRegistry[i].erase(iter);
         break;
      }
   }
   mRegistryMutex.unlock();
}

CTransmitDispatcher::~CTransmitDispatcher()
{
   stop();
   mRegistryMutex.lock();

   mDestroyed = true;

   for (int i = 0; i < PRIO_NUM; ++i)
   {
      for (TSourceList::iterator iter = mRegistry[i].begin(); iter != mRegistry[i].end(); ++iter)
      {
         delete *iter;
      }

      mRegistry[i].clear();
   }

   mRegistryMutex.unlock();
}

void CTransmitDispatcher::threadFunc()
{
   Frame frame;

   LOG4CPLUS_TRACE(logger, "CTransmitDispatcher::threadFunc()");
   while (false == getStopFlag())
   {
      mRegistryMutex.lock();

      if (mDestroyed)
      {
         mRegistryMutex.unlock();
         break;
      }

      for (int i = 0; i < PRIO_NUM; ++i)
      {
         if (!mRegistry[i].empty())
         {
            for (TSourceList::iterator iter = mRegistry[i].begin(); iter != mRegistry[i].end(); ++iter)
            {
               if (static_cast<CSource*>(*iter)->hasData())
               {
                  if (ERR_OK == (*iter)->OnDispatch(frame, getQuoteBySourceType(static_cast<TChannelPriority> (i))))
                  {
                     mpTransmittedFrameQueue->lockQueue();
                     if (mpCarrier->sendFrame(&frame) == ERR_OK)
                     {
                        mpTransmittedFrameQueue->enqueue(frame);
                     }
                     mpTransmittedFrameQueue->unlockQueue();

                     if (static_cast<TChannelPriority> (i) == ePlainData)
                     {
                        mRegistry[i].insert(mRegistry[i].end(), static_cast<CSource*> (*iter));
                        mRegistry[i].erase(iter);
                        break;
                     }
                  }
               }
            }
         }

         // retransmission loop
         Frame* pFrame;
         while ((pFrame = mpTransmittedFrameQueue->getFrameToRetransmit()) != NULL)
         {
            LOG4CPLUS_INFO(logger, "CTransmitDispatcher::threadFunc() retransmit frame chID = "
                  + convertIntegerToString(pFrame->mFrameHeader.channel_id) + ", num = "
                  + convertIntegerToString(pFrame->mFrameHeader.number));
            mpCarrier->sendFrame(pFrame);
            mpTransmittedFrameQueue->reenqueueRetransmittedFrame(pFrame);
         }


      }
      mRegistryMutex.unlock();
      //CThread::sleep(300);
      sched_yield();
   }

   LOG4CPLUS_INFO(logger, "CTransmitDispatcher::threadFunc() completed");
}

UInt32 CTransmitDispatcher::getQuoteBySourceType(TChannelPriority type)
{
   switch (type)
   {
   case eSafetyCritical:
      return SC_QUOTE;
   case eRealTime:
      return RT_QUOTE;
   case ePlainData:
      return PD_QUOTE;
   default:
      return 0xFFFFFFFF;
   }
}
void CTransmitDispatcher::replaceCarrier(CCarrierAdapter* pCarrierAdaptor)
{
   mpCarrier = pCarrierAdaptor;
}
