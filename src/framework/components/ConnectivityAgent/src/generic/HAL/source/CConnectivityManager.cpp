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
* @file             CConnectivityManager.cpp
* @brief            The CConnectivityManager class implementation file  
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
#include "../include/CIncomeDispatcher.hpp"
#include "../include/CTransmitDispatcher.hpp"
#include "../include/CQoS.hpp"
#include "../include/CCarrierAdapter.hpp"
#include "../include/CTransmittedFramesQueue.hpp"
#include "../include/HALInterface.hpp"
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
 #include "../include/CConnectivityManager.hpp"
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
 #include "../../L0/include/CChannelAllocator.hpp"
 
CConnectivityManager::CConnectivityManager(CCarrierAdapter* pCarrierAdaptor):
mQoS(),
mTransmittedFramesQueue(&mQoS),
mpCarrierAdapter(pCarrierAdaptor),
mIncomeDispatcher(pCarrierAdaptor, &mTransmittedFramesQueue),
mTransmitDispatcher(pCarrierAdaptor, &mTransmittedFramesQueue),
mHALInterface(&mTransmitDispatcher, &mIncomeDispatcher, &mQoS)
{
   CChannelAllocator *pChannelAllocator = CChannelAllocator::getInstance();
   pChannelAllocator->setHALInterface(&mHALInterface);
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

void CConnectivityManager::replaceCarrier(CCarrierAdapter* pCarrierAdaptor)
{
   if (mpCarrierAdapter)
   {
      delete mpCarrierAdapter;
   }
   mpCarrierAdapter = pCarrierAdaptor;
   mpCarrierAdapter->registerFrameReceiver(*(static_cast<IFrameReceiver* >(&mIncomeDispatcher)));
   mIncomeDispatcher.replaceCarrier(mpCarrierAdapter);
   mTransmitDispatcher.replaceCarrier(mpCarrierAdapter);
}

CCarrierAdapter*  CConnectivityManager::getCarrier()
{
   return mpCarrierAdapter ;
}

void CConnectivityManager::destroy()
{
   this->~CConnectivityManager();
}

HALInterface * CConnectivityManager::getHALInterface ()
{
   return &mHALInterface;
}
