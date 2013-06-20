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







#ifndef IBASIC_SAMPLE_PROFILE_API_HPP
#define IBASIC_SAMPLE_PROFILE_API_HPP

#ifdef __APPLE__
#include <iviLink/BaseProfileApi.hpp>
#else
//Profile Api Base header, must be included
#include "BaseProfileApi.hpp"
#endif

// IBasicSampleProfile_API class (abstract class describing profile API) inherits CProfileApiBase class
class IBasicSampleProfile_API : public iviLink::BaseProfileApi
{
   // Macro returns the argument 
   IVILINK_PROFILE_API_UID(BasicSampleProfile_PAPI_UID)
   public:

      // Callbacks class (abstract class describing callbacks) inherits IProfileCallbackProxy class
      class Callbacks : public iviLink::Profile::IProfileCallbackProxy
      {
         public:

	    /**
             * Callback that should be invoked when paired profile requested from other side
             * @param a, b - operands
             * @return none
             */
            virtual void operandsReceived(int a, int b) = 0;

	    /**
             * Callback that should be invoked when paired profile requested from other side 
             * @param a - result of operation
             * @return none
             */
            virtual void resultReceived(int a) = 0;
      };

      /**
       * Sending the summands
       * @param a, b - operands
       * @return none
       */
      virtual void sendOperands(int a, int b) = 0;

      /**
       * Sending the obtained result
       * @param a - result of operation
       * @return none
       */
      virtual void sendResult(int a) = 0;
}; 


#endif /* IBASIC_SAMPLE_PROFILE_API_HPP */
