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







#ifndef CBASICSAMPLE_HPP_
#define CBASICSAMPLE_HPP_

//CApp header, must be included for interaction with apllication manager
#ifdef __APPLE__
#include <iviLink/Application.hpp>
#include <iviLink/Logger.hpp>
#include <iviLink/CSignalSemaphore.hpp>
#else
#include "Application.hpp"
#include "Logger.hpp"
#include "CSignalSemaphore.hpp"
#endif
//basic sample api profile header, must be included for implementing callbacks
#include "IBasicSampleProfileAPI.hpp"


// CBasicSample class inherits CApp class
class CBasicSample : public iviLink::Application
{
   // BasicSampleProfileCallbacks class inherits IBasicSampleProfile_API::Callbacks class which describing callbacks
   class BasicSampleProfileCallbacks : public IBasicSampleProfile_API::Callbacks
   {
      CSignalSemaphore * mpSemaphore;
      public:
         explicit BasicSampleProfileCallbacks(CSignalSemaphore & semaphore)
            : mpSemaphore(&semaphore)
         {}


         virtual void handleError(BaseError const & error)
         {
         }

	      /**
          * Callback that should be invoked when paired profile requested from other side
          * @param a, b - operands
          * @return none
          */
         virtual void operandsReceived(int a, int b)
         {
#ifndef __APPLE__
             mpSemaphore->signal();
#else 
             IosLog::Logger::getInstance("BasicSampleCallbacks").info("Got operands: " +
                                                                      convertIntegerToString(a)
                                                                      + " " + convertIntegerToString(b),
                                                                      __FILE__, __func__, __LINE__);
#endif
         }

	      /**
          * Callback that should be invoked when paired profile requested from other side 
          * @param a - result of operation
          * @return none
          */
         virtual void resultReceived(int a)
         {
             LOG4CPLUS_DEBUG(Logger::getInstance("BasicSampleCallbacks"), __PRETTY_FUNCTION__);
#ifndef __APPLE__
             mpSemaphore->signal();
#else
             IosLog::Logger::getInstance("BasicSampleCallbacks").info("Got result: " +
                                                                      convertIntegerToString(a),
                                                                      __FILE__, __func__, __LINE__);
#endif
         }
   };

public:
   CBasicSample(CSignalSemaphore & semaphore);
   virtual ~CBasicSample();

   /**
    * Initializes application in iviLink core
    */
   void init();

   /**
    * Callback that should be invoked by application library thread 
    * after initialization application in application manager 
    * @param launcher shows the initiator of application start
    * @return none
    */
   virtual void onInitDone(iviLink::ELaunchInfo launcher);
    
    // for iOS
    virtual void onLinkUp();
    
private:

   //instance of class implementing callbacks
   BasicSampleProfileCallbacks mBasicSampleCallbacks;
   IBasicSampleProfile_API* mpBasicSampleAPI;

   static Logger msLogger;
   CSignalSemaphore * mpSemaphore;  
};

#endif /* CBASICSAMPLE_HPP_ */
