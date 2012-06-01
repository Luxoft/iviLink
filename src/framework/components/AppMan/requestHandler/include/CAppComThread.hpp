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
 * @file             CAppComThread.hpp
 * @brief            Header file for CAppComThread class
 * @author           VPlachkov
 ***************************************************************************/

#ifndef CAPPCOMTHREAD_HPP_
#define CAPPCOMTHREAD_HPP_

#include <list>
#include <map>

#include "utils/misc/include/Logger.hpp"
#include "framework/libraries/AppMan/Pmp/include/IPmpHandler.hpp"
#include "framework/components/AppMan/requestHandler/include/IAppComThread.hpp"
#include "framework/components/AppMan/requestHandler/include/IReqHandlerForAppCom.hpp"
#include "utils/threads/include/CThread.hpp"
#include "framework/libraries/AppMan/AmpForApp/include/IAppManProtoAmpToApp.hpp"

class CMutex;
class CSignalSemaphore;

namespace AXIS
{

   namespace AMP
   {

      /**
       * This class implements IAppConthread and inherits CThread class to make operations in
       * separate thread
       */
      class CAppComThread  : public CThread
                           , public IAppComThread
      {
      public:
         /**
          * Constructor
          */
         CAppComThread();

         /**
          * Destructor
          */
         virtual ~CAppComThread();

         /**
          * Inits handler for PMP requests
          */
         void initPmpHandler(AppMan::IPmpHandler * pHandler);

         /**
          * Inits handler for app requests
          */
         void initAppHandler(AppMan::Ipc::IAppManProtoAmpToApp * pApp);

         /**
          * Inits handler of IReqHandlerForAppCom interface
          */
         void initAppComHandler(IReqHandlerForAppCom * pHandlerForAppCom);

         /**
          * Uninits CAppComThread
          */
         void uninit();

         /**
          * Stops handling requests
          */
         void stopRequests();

      private:
         /*
          * Request to PMP to say about session problems (error while session is starting)
          * @param session is UID of session
          */
         virtual void addRequest(AXIS::Service::SessionUid session);

         /*
          * Request to app to start new session
          * @param pid is pid of app process
          * @param session is UID of session
          * @param service is UID of service
          */
         virtual void addSessionRequest(pid_t pid, AXIS::Service::SessionUid session,
               AXIS::Service::Uid service);

         /*
          * Makes request to app to get launch info
          * pid is pid of app process
          */
         virtual void addAppLaunchInfoRequest(pid_t pid);

         /**
          * This function is called in new thread after start
          */
         virtual void threadFunc();

         /**
          * Returns true if there are requests to process
          */
         bool hasRequests();

         /**
          * Processes requests
          */
         void handleRequest();

         AppMan::IPmpHandler * mpPmp; ///< Pointer to PMP protocol
         bool mBe; ///< while it's true, requests are processed
         std::list<Service::SessionUid> mRequests; ///< list of requests to PMP
         CMutex * mpReqMutex; ///< mutex for mRequests
         CSignalSemaphore * mpSignalSemaphore; ///< semaphore is used to wait new requests
         AppMan::Ipc::IAppManProtoAmpToApp * mpApp; ///< pointer to app protocol
         std::map<pid_t, std::pair<Service::SessionUid, Service::Uid> > mSessionRequests; ///< map of service UIDs
         std::list<pid_t> mAppLaunchRequests; ///< requests for getting app launch information
         IReqHandlerForAppCom * mpHandlerForAppCom; ///< pointer to handler of setAppLaunchInfo functionality

         static Logger msLogger; ///< object of logger
      };

   }

}

#endif /* CAPPCOMTHREAD_HPP_ */
