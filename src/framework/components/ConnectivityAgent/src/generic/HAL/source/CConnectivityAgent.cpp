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
#include <assert.h>
#include <cstdio>
#include <cstring>

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "CConnectivityManager.hpp"
#include "CCarrierAdapter.hpp"
#include "ConnectionInfo.hpp"
#include "CAdapterFactory.hpp"
#include "CDummyConnectionFinder.hpp"
#include "CTcpAutoConnectionFinder.hpp"

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CConnectivityAgent.hpp"

#include "utils/misc/include/Logger.hpp"

Logger CConnectivityAgent::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CConnectivityAgent"));

CConnectivityAgent::CConnectivityAgent(bool isServer) :
   CThread("CConnectivityAgent"),
   mpManager(NULL),
   mIsServer(isServer),
   mpDummyConnectionFinder(NULL),
	mpTcpAutoConnectionFinder(NULL)
{
   memset(mConnectionEstablished, 0, sizeof(mConnectionEstablished));
}

CConnectivityAgent::~CConnectivityAgent()
{
   stop();
   disconnect();
   if (mpManager)
   {
      mpManager->destroy();
      mpManager = 0;
   }

   delete mpTcpAutoConnectionFinder;
   delete mpDummyConnectionFinder;
}

void CConnectivityAgent::threadFunc()
{
   LOG4CPLUS_TRACE(logger, "agent: threadFunc");
   while (false == getStopFlag())
   {
      mMainFuncMutex.lock();

      if (mpManager == 0 || !mConnectionEstablished[eTcpAutoCarrier])
      {
         checkNewCarriers();
      }
      else
      {
         CCarrierAdapter* pCarrier = getCurrentCarrierAdapter();

         if (pCarrier && pCarrier->isBroken())
         {
            if (mConnectionEstablished[eTcpAutoCarrier])
            {
               mConnectionEstablished[eTcpAutoCarrier] = false;
            }
            if (mpTcpAutoConnectionFinder)
            {
               delete mpTcpAutoConnectionFinder;
               mpTcpAutoConnectionFinder = NULL;
            }
         }
      }
      mMainFuncMutex.unlock();

      CThread::sleep(2000);
   }
   LOG4CPLUS_INFO(logger, "agent: stopped");
}

void CConnectivityAgent::checkNewCarriers()
{
   LOG4CPLUS_TRACE(logger, "agent: checkNewCarriers()");

   if (!mConnectionEstablished[eTcpAutoCarrier])
   {
      if (initNewTcpAutoConnection())
      {
         mConnectionEstablished[eTcpAutoCarrier] = true;
         return;
      }
   }

   LOG4CPLUS_INFO(logger, "agent: not found");
}


CCarrierAdapter* CConnectivityAgent::initNewDummyConnection()
{
   LOG4CPLUS_TRACE(logger, "agent: initNewDummyConnection");

   if (!mpDummyConnectionFinder)
   {
      LOG4CPLUS_INFO(logger, "agent: client CDummmyConnectionFinder started");
      mpDummyConnectionFinder = new CDummyConnectionFinder(&mSigSem, mIsServer);
   }

   assert(mpDummyConnectionFinder);
   if (mpDummyConnectionFinder->isReady())
   {
      CCarrierAdapter* pAdapter = mpDummyConnectionFinder->getConnectedAdapter();
      assert(pAdapter);

      if (initAdapterHandshake(pAdapter))
      {
         return pAdapter;
      }
      else
      {
         delete pAdapter;
      }
   }

   return NULL;
}

CCarrierAdapter* CConnectivityAgent::initNewTcpAutoConnection()
{
   LOG4CPLUS_TRACE(logger, "agent: initNewTcpAutoConnection");

   if (!mpTcpAutoConnectionFinder)
   {
      LOG4CPLUS_INFO(logger, "agent: client CTcpAutoConnectionFinder started");
      mpTcpAutoConnectionFinder = new CTcpAutoConnectionFinder(&mSigSem, mIsServer);
   }

   assert(mpTcpAutoConnectionFinder);
   if (mpTcpAutoConnectionFinder->isReady())
   {
      CCarrierAdapter* pAdapter = mpTcpAutoConnectionFinder->getConnectedAdapter();
      assert(pAdapter);

      if (initAdapterHandshake(pAdapter))
      {
         return pAdapter;
      }
      else
      {
         delete pAdapter;
      }
   }

   return NULL;
}

bool CConnectivityAgent::initAdapterHandshake(CCarrierAdapter* pAdapter)
{
   LOG4CPLUS_INFO(logger, "agent: begin handshake");
   if (pAdapter->makeHandshake() != ERR_OK)
   {
      LOG4CPLUS_INFO(logger, "CConnectivityAgent::initAdapterHandshake: handshake failed");
      return false;
   }
   LOG4CPLUS_INFO(logger, "CConnectivityAgent::initAdapterHandshake: handshake successful");

   mManagerMutex.lockWrite();

   if (!mpManager)
   {
      mpManager = new CConnectivityManager(pAdapter);
      LOG4CPLUS_INFO(logger, "CConnectivityAgent::initAdapterHandshake: created Connectivity Manager");
   }
   else
   {
      LOG4CPLUS_INFO(logger, "CConnectivityAgent::initAdapterHandshake: replace Carrier in  Connectivity Manager");
      mpManager->replaceCarrier(pAdapter);
   }
   mManagerMutex.unlock();

   return true;
}

void CConnectivityAgent::disconnect()
{
   LOG4CPLUS_INFO(logger, "agent: CConnectivityAgent::disconnect()");

   if (mpManager)
   {
      mpManager->destroy();
   }
}

void CConnectivityAgent::onConnectionProblem(ECarrierType carrierType)
{
   LOG4CPLUS_INFO(logger, "agent: CHALController::onConnectionLost(" +
		   convertIntegerToString((int)carrierType) + ")");

   CCarrierAdapter* pAdapter = tryToRecover(carrierType);
   if (pAdapter)
   {
      mpManager->replaceCarrier(pAdapter);

      LOG4CPLUS_INFO(logger, "agent: CHALController::onReconnected("
            +  convertIntegerToString((int)carrierType) + ")");
   }
   else
   {
      disconnect();
   }
}

CCarrierAdapter* CConnectivityAgent::tryToRecover(ECarrierType carrierType)
{
   CCarrierAdapter* pNewAdapter = 0;
   if (carrierType == eDummyTcpCarrier )
   {
      pNewAdapter = initNewDummyConnection();
      if (pNewAdapter)
      {
         return pNewAdapter;
      }
      else
      {
         mConnectionEstablished[eDummyTcpCarrier] = false;
      }
   }

   LOG4CPLUS_INFO(logger, "agent: cannot recover broken connection");

   return 0;
}
CCarrierAdapter* CConnectivityAgent::getCurrentCarrierAdapter()
{
   CCarrierAdapter* pCA = NULL;
   mManagerMutex.lockRead();
   if (mpManager)
   {
      pCA = mpManager->getCarrier();
   }
   mManagerMutex.unlock();

   return pCA;
}
HALInterface * CConnectivityAgent::getHALInterface ()
{
   HALInterface* pHI = NULL;
   mManagerMutex.lockRead();
   if (mpManager)
   {
      pHI = mpManager->getHALInterface();
   }
   mManagerMutex.unlock();

   return pHI;
}
