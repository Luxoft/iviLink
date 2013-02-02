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


#ifndef SYSTEMCONTROLLERMSG_AM_HPP_
#define SYSTEMCONTROLLERMSG_AM_HPP_

#include "SystemControllerMsgProxy.hpp"
#include "Logger.hpp"
#include <tr1/functional>

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

          void setLinkCallbacks( std::tr1::function<void ()> link_up,
                                   std::tr1::function<void ()> link_down);

      private:
         /**
          * Callback is used to exit
          */
         virtual BaseError onShutDown();

          virtual void onLinkUpNotification();
          virtual void onLinkDownNotification();


         /**
          * Pointer to signal semaphore object
          */
         CSignalSemaphore * mpSignalSemaphore;

         /**
          * Object of logger
          */
         static Logger msLogger;

          std::tr1::function<void ()> link_up, link_down;
    };

   }

}

#endif /* SYSTEMCONTROLLERMSG_AM_HPP_ */
