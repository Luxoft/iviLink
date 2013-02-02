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


#include <map>

#include "CCarrierAdapter.hpp"
#include "CIncomeDispatcher.hpp"
#include "CTransmitDispatcher.hpp"
#include "CQoS.hpp"
#include "CTransmittedFramesQueue.hpp"

 #include "CConnectivityManager.hpp"
 #include "CChannelAllocator.hpp"
 
using namespace iviLink::ConnectivityAgent::L0;
using iviLink::ConnectivityAgent::L1::CChannelAllocator;


CConnectivityManager::CConnectivityManager(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdaptor):
   mpQoS(new CQoS()),
   mpTransmittedFramesQueue(new CTransmittedFramesQueue(mpQoS)),
   mpCarrierAdapter(pCarrierAdaptor),
   mpIncomeDispatcher(new CIncomeDispatcher(pCarrierAdaptor, mpTransmittedFramesQueue)),
   mpTransmitDispatcher(new CTransmitDispatcher(pCarrierAdaptor, mpTransmittedFramesQueue)),
   mL0Interface(mpTransmitDispatcher, mpIncomeDispatcher, mpQoS)
{
   CChannelAllocator *pChannelAllocator = CChannelAllocator::getInstance();
   pChannelAllocator->setL0Interface(&mL0Interface);
   pChannelAllocator->OnConnected();
}

CConnectivityManager::~CConnectivityManager()
{
   if (mpCarrierAdapter)
   {
      delete mpCarrierAdapter;
   }
   mpCarrierAdapter = NULL;
   CChannelAllocator::getInstance()->OnDisconnected();
}

void CConnectivityManager::replaceCarrier(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdaptor)
{
   if (mpCarrierAdapter)
   {
      delete mpCarrierAdapter;
   }
   mpCarrierAdapter = pCarrierAdaptor;
   mpCarrierAdapter->registerFrameReceiver(*(static_cast<IFrameReceiver* >(mpIncomeDispatcher)));
   mpIncomeDispatcher->replaceCarrier(mpCarrierAdapter);
   mpTransmitDispatcher->replaceCarrier(mpCarrierAdapter);
}

iviLink::ConnectivityAgent::HAL::CCarrierAdapter*  CConnectivityManager::getCarrier()
{
   return mpCarrierAdapter ;
}

void CConnectivityManager::destroy()
{
   this->~CConnectivityManager();
}

L0Interface * CConnectivityManager::getL0Interface ()
{
   return &mL0Interface;
}

void CConnectivityManager::OnDisconnected()
{
   CChannelAllocator::getInstance()->OnDisconnected();
}
