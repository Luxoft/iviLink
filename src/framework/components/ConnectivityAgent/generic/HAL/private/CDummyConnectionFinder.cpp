/* 
 * 
 * iviLINK SDK, version 1.0.1
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
#include "CDummyConnectionFinder.hpp"

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CAdapterFactory.hpp"
#include "ConnectionInfo.hpp"
#include "CTcpCarrierAdapter.hpp"
#include "EGenderType.hpp"
#include "utils/misc/Logger.hpp"

using namespace iviLink::ConnectivityAgent::HAL;

Logger CDummyConnectionFinder::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CDummyConnectionFinder"));

CDummyConnectionFinder::CDummyConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender) :
   CConnectionFinder(handler, gender)
{
   switch (gender)
   {
   case eServerGender:
   case eClientGender:
      break;
   case eAnyGender:
      assert(false && "eAny is not allowed as gender");
      break;
   }
}


CDummyConnectionFinder::~CDummyConnectionFinder()
{

}

void CDummyConnectionFinder::performSearch()
{
   CTcpConnectionInfo info;
   info.serverSocket = (mGenderType == eServerGender);
   if (mGenderType == eServerGender)
   {
      info.host = std::string("0.0.0.0");
   }
   else
   {
      info.host = std::string("localhost");
   }

   info.service = std::string(AXIS_PORT);

   CCarrierAdapter* pAdapter = NULL;
   if (mGenderType == eServerGender)
   {
      pAdapter = iviLink::ConnectivityAgent::HAL::CAdapterFactory::CreateTcpAdapter(info);
      LOG4CPLUS_INFO(logger, "dummFinder: starting server adapter");
      static_cast<CTcpCarrierAdapter*>(pAdapter)->connect();
      while (pAdapter->isBroken())
      {
         CThread::sleep(2000);
      }
      setReady(pAdapter);
   }
   else
   {
      while (true)
      {
         pAdapter = CAdapterFactory::CreateTcpAdapter(info);
         static_cast<CTcpCarrierAdapter*>(pAdapter)->connect();
         if (pAdapter->isBroken())
         {
            delete pAdapter;
         }
         else
         {
            break;
         }

         CThread::sleep(2000);
      }
      setReady(pAdapter);
   }


}

