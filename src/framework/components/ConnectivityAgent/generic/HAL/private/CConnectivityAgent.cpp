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
#include <unistd.h>
#include <assert.h>
#include <cstdio>
#include <cstring>

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "ConnectionInfo.hpp"
#include "CAdapterFactory.hpp"
#include "CDummyConnectionFinder.hpp"
#include "CTcpAutoConnectionFinder.hpp"
#include "framework/components/ConnectivityAgent/generic/L0/CConnectivityManager.hpp"
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CConnectivityAgent.hpp"

#include "utils/misc/Logger.hpp"

using namespace iviLink::ConnectivityAgent::HAL;
using iviLink::ConnectivityAgent::L0::CConnectivityManager;

Logger CConnectivityAgent::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CConnectivityAgent"));

CConnectivityAgent::CConnectivityAgent(bool isServer) :
   CThread("CConnectivityAgent", true),
   mConnectionEstablished(false),
   mpManager(NULL),
   mGenderType(eAnyGender)
{
   mFinders.push_back(new CTcpAutoConnectionFinder(*this, eAnyGender));
   //mFinders.push_back(new CDummyConnectionFinder(*this, eAnyGender));
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

   for (tFinders::iterator it = mFinders.begin(); it != mFinders.end(); ++it)
   {
      CConnectionFinder* cf = *it;
      cf->stopSearch();
      delete cf;
   }
   mFinders.clear();
}

void CConnectivityAgent::threadFunc()
{
   LOG4CPLUS_TRACE_METHOD(logger, "agent: threadFunc");



   while (false == getStopFlag())
   {
      mMainFuncMutex.lock();

      if (!mpManager || !mConnectionEstablished)
      {
         for (tFinders::iterator iter = mFinders.begin(); iter != mFinders.end(); ++iter)
         {
            (*iter)->startSearch();
         }
      }
      else
      {
         CCarrierAdapter* pCarrier = getCurrentCarrierAdapter();

         if (!pCarrier || pCarrier->isBroken())
         {
            LOG4CPLUS_TRACE(logger, "agent: connection lost");
            mConnectionEstablished = false;
            mGenderType = eAnyGender;

            if (this->mpManager)
            {
               LOG4CPLUS_TRACE(logger, "agent: calling mpManager->OnDisconnected ");
               mpManager->OnDisconnected();
            }
         }
      }
      mMainFuncMutex.unlock();

      CThread::sleep(2000);
   }
   LOG4CPLUS_INFO(logger, "agent: stopped");
}

void CConnectivityAgent::connectionFound(CConnectionFinder* pFinder)
{
   LOG4CPLUS_TRACE_METHOD(logger, std::string(__PRETTY_FUNCTION__)
      + " pFinder = "
      + convertIntegerToString(reinterpret_cast<intptr_t>(pFinder)));

   mMainFuncMutex.lock();

   if (pFinder->getState() == CConnectionFinder::eWaitAck)
   {
      if (mConnectionEstablished)
      {
         /// @todo implement this case
         assert(false && "not implemented");
      }
      else
      {
         CCarrierAdapter* pAdapter = pFinder->getConnectedAdapter();
         assert(pAdapter);

         if (initAdapterHandshake(pAdapter))
         {
            mConnectionEstablished = true;

            for (tFinders::iterator it = mFinders.begin(); it != mFinders.end(); ++it)
            {
               (*it)->stopSearch();
            }
         }
         else
         {
            delete pAdapter;
            pFinder->continueSearch();
         }
      }
   }


   mMainFuncMutex.unlock();
}

bool CConnectivityAgent::initAdapterHandshake(CCarrierAdapter* pAdapter)
{
   LOG4CPLUS_INFO(logger, "CConnectivityAgent::initAdapterHandshake: begin handshake adapter = "
      + convertIntegerToString(reinterpret_cast<intptr_t>(pAdapter)));

   if (pAdapter->makeHandshake() != ERR_OK)
   {
      LOG4CPLUS_INFO(logger, "CConnectivityAgent::initAdapterHandshake: handshake failed");
      return false;
   }
   LOG4CPLUS_INFO(logger, "CConnectivityAgent::initAdapterHandshake: handshake successful");

   mManagerMutex.lockWrite();

   mGenderType = pAdapter->getGender();

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
   mManagerMutex.unlockWrite();

   return true;
}

void CConnectivityAgent::disconnect()
{
   LOG4CPLUS_INFO(logger, "agent: CConnectivityAgent::disconnect()");

   if (mpManager)
   {
      mpManager->destroy();
      mpManager = NULL;
   }

   mGenderType = eAnyGender;
}

void CConnectivityAgent::onConnectionProblem(ECarrierType carrierType)
{
//   LOG4CPLUS_INFO(logger, "agent: CHALController::onConnectionLost(" +
//      convertIntegerToString((int)carrierType) + ")");
//
//   CCarrierAdapter* pAdapter = tryToRecover(carrierType);
//   if (pAdapter)
//   {
//      mpManager->replaceCarrier(pAdapter);
//
//      LOG4CPLUS_INFO(logger, "agent: CHALController::onReconnected("
//            +  convertIntegerToString((int)carrierType) + ")");
//   }
//   else
//   {
//      disconnect();
//   }
}

iviLink::ConnectivityAgent::HAL::CCarrierAdapter* CConnectivityAgent::tryToRecover(ECarrierType carrierType)
{
//   CCarrierAdapter* pNewAdapter = 0;
//   if (carrierType == eDummyTcpCarrier )
//   {
//      pNewAdapter = initNewDummyConnection();
//      if (pNewAdapter)
//      {
//         return pNewAdapter;
//      }
//      else
//      {
//         mConnectionEstablished[eDummyTcpCarrier] = false;
//      }
//   }
//
   LOG4CPLUS_INFO(logger, "agent: cannot recover broken connection");

   return 0;
}
iviLink::ConnectivityAgent::HAL::CCarrierAdapter* CConnectivityAgent::getCurrentCarrierAdapter()
{
   CCarrierAdapter* pCA = NULL;
   mManagerMutex.lockRead();
   if (mpManager)
   {
      pCA = mpManager->getCarrier();
   }
   mManagerMutex.unlockRead();

   return pCA;
}

iviLink::ConnectivityAgent::L0::L0Interface * CConnectivityAgent::getL0Interface ()
{
   iviLink::ConnectivityAgent::L0::L0Interface* pInterface = NULL;
   mManagerMutex.lockRead();
   if (mpManager)
   {
      pInterface = mpManager->getL0Interface();
   }
   mManagerMutex.unlockRead();

   return pInterface;
}


EGenderType CConnectivityAgent::getCurrentGender() const
{
   return mGenderType;
}

