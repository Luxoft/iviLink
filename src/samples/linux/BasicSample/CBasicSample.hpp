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



#ifndef CBASICSAMPLE_HPP_
#define CBASICSAMPLE_HPP_

//CApp header, must be included for interaction with apllication manager
#include "framework/appLib/appLib/CApp.hpp"
#include "utils/misc/Logger.hpp"
//basic sample api profile header, must be included for implementing callbacks
#include "samples/linux/Profiles/ProfileAPI/IBasicSampleProfileAPI.hpp"
#include "utils/threads/CSignalSemaphore.hpp"

// CBasicSample class inherits CApp class
class CBasicSample : public iviLink::CApp
{
   // BasicSampleProfileCallbacks class inherits IBasicSampleProfile_API::Callbacks class which describing callbacks
   class BasicSampleProfileCallbacks : public IBasicSampleProfile_API::Callbacks
   {
      CSignalSemaphore * mpSemaphore;
      public:
         explicit BasicSampleProfileCallbacks(CSignalSemaphore & semaphore)
            : mpSemaphore(&semaphore)
         {}

         virtual void handleError(CError const & error)
         {
         }

	 /**
          * Callback that should be invoked when paired profile requested from other side
          * @param a, b - operands
          * @return none
          */
         virtual void operandsReceived(int a, int b)
         {
            mpSemaphore->signal();
         }

	 /**
          * Callback that should be invoked when paired profile requested from other side 
          * @param a - result of operation
          * @return none
          */
         virtual void resultReceived(int a)
         {
            mpSemaphore->signal();
         }
   };

public:
   CBasicSample(CSignalSemaphore & semaphore);
   virtual ~CBasicSample();

   /**
    * Callback that should be invoked by application library thread 
    * after initialization application in application manager 
    * @param launcher shows the initiator of application start
    * @return none
    */
   virtual void initDone(iviLink::ELaunchInfo launcher);

private:

   //instance of class implementing callbacks
   BasicSampleProfileCallbacks mBasicSampleCallbacks;
   IBasicSampleProfile_API* mpBasicSampleAPI;

   static Logger msLogger;
   CSignalSemaphore * mpSemaphore;
};

#endif /* CBASICSAMPLE_HPP_ */
