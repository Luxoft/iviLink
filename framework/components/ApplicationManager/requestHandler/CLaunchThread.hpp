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


#ifndef CLAUNCHTHREAD_HPP_
#define CLAUNCHTHREAD_HPP_

#include "Logger.hpp"
#include "CThread.hpp"
#include "UID.hpp"
#include "ILauncher.hpp"
#include "ILauncherHandler.hpp"
#include "IDatabase.hpp"
#include "IReqHandlerForLaunch.hpp"
#include "ILaunchThread.hpp"

class CSignalSemaphore;
class CMutex;

namespace iviLink
{

   namespace AMP
   {

      /**
       * This class is a thread that launches applications
       */
      class CLaunchThread  : public CThread
                           , public ILaunchThread
                           , public ILauncherHandler
      {
      public:
         /**
          * Constructor
          */
         CLaunchThread();

         /**
          * Destructor
          */
         virtual ~CLaunchThread();

         /**
          * inits launcher
          * @param pLauncher is pointer to launcher
          */
         void initLauncher(ILauncher * pLauncher);

         /**
          * Inits database
          * @param pDatabase is pointer to database
          */
         void initDb(IDatabase * pDatabase);

         /**
          * Inits request handler
          * @param pReqHandler is pointer to request handler object
          */
         void initRequestHandler(IReqHandlerForLauncher * pReqHandler);

         /**
          * Uninits launch thread
          */
         void uninit();

         /**
          * Stops thread
          */
         void stopLaunching();

         /**
          * Launches application
          * @param session is UID of session
          * @param dbId is id of app in database
          */
         virtual void launchApp(iviLink::Service::SessionUid session, int dbId);

      private:
         /**
          * it's an entry point for thread that processes requests
          */
         virtual void threadFunc();

         /**
          * returnes true if it had requests to launch app
          */
         bool hasRequests();

         /**
          * handles requests
          */
         void handleRequest();

         /**
          * callback from launcher
          * @param launchInfo is string with information for launching
          * @pid of started app
          */
         virtual void launchedApp(const std::string & launchInfo, pid_t pid);

         /**
          * callback from launcher, called in case of launching problems
          * @param launchInfo is string with information for launching
          */
         virtual void launchError(const std::string & launchInfo);

         ILauncher * mpLauncher; ///< pointer to launcher object
         bool mBe;   ///< true it's no requests to stop thread
         CSignalSemaphore * mpSignalSemaphore; ///< pointer to semaphore
         IDatabase * mpDatabase; ///< pointer to AppMan database
         IReqHandlerForLauncher * mpReqHandler; ///< pointer to request handler interface for launcher
         std::map<std::string, Service::SessionUid> mSessions; ///< map of sessions
         CMutex * mpSessionMutex; ///< mutex for mSessions
         std::list<std::pair<Service::SessionUid, int> > mRequests; ///< list of app IDs
         CMutex * mpReqMutex; ///< mutex for mRequests

         static Logger msLogger; ///< object of logger
      };

   }

}

#endif /* CLAUNCHTHREAD_HPP_ */

