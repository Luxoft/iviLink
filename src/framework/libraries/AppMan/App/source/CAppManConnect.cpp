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
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             CAppManConnect.cpp
 * @brief            Source file for CAppManConnect
 * @author           VPlachkov
 ***************************************************************************/

#include "framework/libraries/AppMan/App/include/CAppManConnect.hpp"

#include <cassert>

namespace AXIS
{

   namespace AppMan
   {

      namespace Ipc
      {
         Logger CAppManConnect::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.App.Connect"));

         CAppManConnect::CAppManConnect(CAppManProtoClient * pClient)
         : CThread("AppManAppConnection")
         , mpClient(pClient)
         {
            LOG4CPLUS_TRACE(msLogger,"CAppManConnect()");
            assert(mpClient);
         }

         CAppManConnect::~CAppManConnect()
         {
            LOG4CPLUS_TRACE(msLogger,"~CAppManConnect()");
         }

         void CAppManConnect::connect()
         {
            LOG4CPLUS_TRACE(msLogger,"connect()");
            start();
         }

         void CAppManConnect::disconnect()
         {
            LOG4CPLUS_TRACE(msLogger,"disconnect()");
            mpClient->disconnect();
         }

         void CAppManConnect::threadFunc()
         {
            LOG4CPLUS_TRACE(msLogger,"threadFunc()");
            mpClient->loop();
         }

      }

   }

}
