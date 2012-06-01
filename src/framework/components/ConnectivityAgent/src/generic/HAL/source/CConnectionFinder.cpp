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
 * @file             CConnectionFinder.cpp
 * @brief            CConnectionFinder class implementation file
 * @author           PIlin
 ***************************************************************************/

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <assert.h>

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "../include/CCarrierAdapter.hpp"

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "../include/CConnectionFinder.hpp"

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/misc/include/Logger.hpp"


Logger CConnectionFinder::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CConnectionFinder"));

CConnectionFinder::CConnectionFinder(CSignalSemaphore* pSem, bool isServer) :
   CThread("CConnectionFinder"), 
	mpConnAgSigSem(pSem), 
	mIsReady(false), 
	mIsServer(isServer),
	mpAdapter(NULL)
{
   assert(pSem);
}

CConnectionFinder::~CConnectionFinder()
{
   // In case, that adapter has been created, but no one was interested in it.
   stop();
   mMutex.lockWrite();
   delete mpAdapter;
   mpAdapter = 0;
   mMutex.unlock();
}

bool CConnectionFinder::isReady() const
{
   mMutex.lockRead();
   bool value = mIsReady;
   mMutex.unlock();
   return value;
}

void CConnectionFinder::setReady(CCarrierAdapter* pAdapter)
{
   LOG4CPLUS_INFO(logger, "finder: set ready");
   mMutex.lockWrite();
   mpAdapter = pAdapter;
   mIsReady = true;
   mMutex.unlock();
   mpConnAgSigSem->signal();
}

CCarrierAdapter* CConnectionFinder::getConnectedAdapter()
{
   mMutex.lockWrite();
   CCarrierAdapter* pAdapter = mpAdapter;
   mpAdapter = NULL;
   mIsReady = false;
   mMutex.unlock();
   return pAdapter;
}
