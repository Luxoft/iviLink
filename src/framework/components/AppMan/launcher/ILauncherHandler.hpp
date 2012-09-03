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











#ifndef ILAUNCHERHANDLER_HPP_
#define ILAUNCHERHANDLER_HPP_

namespace iviLink
{
   namespace AMP
   {

      /**
       * This interface is designed for launcher client to handle events in process of launching
       */
      class ILauncherHandler
      {
      public:
         /**
          * Function is used to inform about pid of launched app
          * @param launchInfo of launched application
          * @param pid of launched process
          */
         virtual void launchedApp(const std::string & launchInfo, pid_t pid) = 0;

         /**
          * Function is used to inform abount errors while launching application
          */
         virtual void launchError(const std::string & launchInfo) = 0;

         /**
          * Virtual destructor
          */
         virtual ~ILauncherHandler() {}
      };
   }
}

#endif /* ILAUNCHERHANDLER_HPP_ */
