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
#include "CCarrierAdapter.hpp"
#include "IFoundConnectionHandler.hpp"

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CConnectionFinder.hpp"

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "Logger.hpp"

using namespace iviLink::ConnectivityAgent::HAL;

Logger CConnectionFinder::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CConnectionFinder"));

CConnectionFinder::CConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender) :
   CThread("CConnectionFinder", true),
   mGenderType(gender),
   mFoundConnectionHandler(handler),
   mState(eStopped),
   mpAdapter(NULL)
{
}

CConnectionFinder::~CConnectionFinder()
{
   stopSearch();

   // In case, that adapter has been created, but no one was interested in it.

   mStateCondVar.lock();
   delete mpAdapter;
   mpAdapter = NULL;
   mStateCondVar.unlock();
}

CConnectionFinder::EState CConnectionFinder::getState() const
{
   mStateCondVar.lock();
   EState res = mState;
   mStateCondVar.unlock();
   return res;
}

void CConnectionFinder::startSearch()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mStateCondVar.lock();
   if (mState == eStopped)
   {
      mState = eSearching;
      start();
      mStateCondVar.signal();
   }
   mStateCondVar.unlock();
}

void CConnectionFinder::stopSearch()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mStateCondVar.lock();
   if (mState != eStopped)
   {
      mState = eStopped;

      if (!CThread::isThisThread())
         stop();

      mStateCondVar.signal();
   }
   mStateCondVar.unlock();
}

void CConnectionFinder::continueSearch()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mStateCondVar.lock();
   if (mState == eWaitAck)
   {
      mState = eSearching;
      mStateCondVar.signal();
   }
   mStateCondVar.unlock();
}

void CConnectionFinder::setReady(CCarrierAdapter* pAdapter)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mStateCondVar.lock();
   {
      mpAdapter = pAdapter;
      mState = eWaitAck;
      mStateCondVar.signal();
   }
   mStateCondVar.unlock();

   mFoundConnectionHandler.connectionFound(this);
}

void CConnectionFinder::threadFunc()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   bool stop = false;

   while (!stop && !getStopFlag())
   {
      mStateCondVar.lock();
      {
         while (mState == eWaitAck)
            mStateCondVar.wait();
         if (mState == eStopped)
            stop = true;
      }
      mStateCondVar.unlock();
      if (stop)
         break;

      performSearch();
   }

   assert(mState == eStopped);
   if(mState != eStopped)
   {
      LOG4CPLUS_ERROR(msLogger, "mState != eStopped");
   }
}

CCarrierAdapter* CConnectionFinder::getConnectedAdapter()
{
   CCarrierAdapter* pAdapter = NULL;
   mStateCondVar.lock();
   if (mState == eWaitAck)
   {
      pAdapter = mpAdapter;
      mpAdapter = NULL;
   }
   mStateCondVar.unlock();
   return pAdapter;
}
