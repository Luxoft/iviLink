/* 
 * 
 * iviLINK SDK, version 1.0
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







#ifndef SYSTEMCONTROLLERMSG_AM_HPP_
#define SYSTEMCONTROLLERMSG_AM_HPP_

#include "framework/messageProtocol/SystemController_AppMan/AppMan/SystemControllerMsgProxy.hpp"
#include "utils/misc/Logger.hpp"

class CSignalSemaphore;

namespace iviLink
{

   namespace AMP
   {

      /**
       * This class is used to organize communication of AMP with System Controller
       */
      class SystemControllerMsg : public AppManMsgProtocol::SystemControllerMsgProxy
      {
      public:
         /**
          * Constructor
          * @param pSemaphore is used to save this pointer and signal semaphore in case of exiting
          */
         SystemControllerMsg(CSignalSemaphore * pSemaphore);

         /**
          * Virtual destructor
          */
         virtual ~SystemControllerMsg();

      private:
         /**
          * Callback is used to exit
          */
         virtual CError onShutDown();

         /**
          * Pointer to signal semaphore object
          */
         CSignalSemaphore * mpSignalSemaphore;

         /**
          * Object of logger
          */
         static Logger msLogger;
      };

   }

}

#endif /* SYSTEMCONTROLLERMSG_AM_HPP_ */
