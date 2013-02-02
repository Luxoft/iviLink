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


#include <cstdlib>
#include <cassert>
#include <unistd.h>

#include "SystemControllerMsg.hpp"
#include "CSignalSemaphore.hpp"
#include "Exit.hpp"

namespace iviLink
{

   namespace AMP
   {
      Logger SystemControllerMsg::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.SC"));

      SystemControllerMsg::SystemControllerMsg(CSignalSemaphore * pSemaphore)
      : SystemControllerMsgProxy("SysCtrl_AppMan")
      , mpSignalSemaphore(pSemaphore)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         bool noError = false;
         for (int i = 0; i < 15; ++i)
         {
            LOG4CPLUS_INFO(msLogger, "Try to connect to System Controller number, " + convertIntegerToString(i));
            if (connect().isNoError())
            {
               noError = true;
               break;
            }
            usleep(500000);
         }
         if (!noError)
         {
            LOG4CPLUS_FATAL(msLogger, "Can't connect to system controller");
            killProcess(1);
         }
      }

      SystemControllerMsg::~SystemControllerMsg()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      BaseError SystemControllerMsg::onShutDown()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpSignalSemaphore->signal();
         return BaseError::NoError("","");
      }

       using namespace std::tr1;

       void SystemControllerMsg::setLinkCallbacks( function<void ()> link_up_,
                                                     function<void ()> link_down_)
       {
           link_up= link_up_;
           link_down= link_down_;
       }

       void SystemControllerMsg::onLinkUpNotification()
       {
           if( link_up ) link_up();
       }

       void SystemControllerMsg::onLinkDownNotification()
       {
           if( link_down ) link_down();
       }
   }
}
