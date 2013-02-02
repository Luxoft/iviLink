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


#ifndef ILAUNCHER_HPP_
#define ILAUNCHER_HPP_

#include "ILauncherHandler.hpp"

namespace iviLink
{
   namespace AMP
   {

      /**
       * Interface of AppMan Launcher module functionality
       */
      class ILauncher
      {
      public:
        virtual void init(ILauncherHandler * handler) = 0;
         /**
          * Functions is used to launch app using given launch information
          * @param is string with information needed to launch app, in linux it's a full path of exe
          */
         virtual void launchApplication(const std::string & launchInfo) = 0;

         virtual void deinit() = 0;

         /**
          * Virtual destructor
          */
         virtual ~ILauncher() {}
      };
   }
}

#endif /* ILAUNCHER_HPP_ */
