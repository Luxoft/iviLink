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
 * @file             CRepoController.cpp
 * @brief            Source file for CProfileRepoController class
 * @author           VPlachkov
 ***************************************************************************/

#include "framework/components/ProfileManager/PMP/repository/include/CRepoController.hpp"

namespace AXIS
{

   namespace PMP
   {

      Logger CRepoController::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.CRepoController"));

      CRepoController::CRepoController(conf::Configurator * pConfig)
         : mRepoClient(pConfig)
         , mConnection(&mRepoClient)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mConnection.connect();
      }

      CRepoController::~CRepoController()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mConnection.disconnect();
      }

      ProfileRepository::IProfileRepoClient * CRepoController::repository()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return &mRepoClient;
      }

      CRepoController * CRepoController::instance(conf::Configurator * pConfig/* = 0*/)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         static CRepoController sInstance(pConfig);
         return &sInstance;
      }

      bool CRepoController::checkConnection() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return mRepoClient.checkConnection();
      }

   }

}
