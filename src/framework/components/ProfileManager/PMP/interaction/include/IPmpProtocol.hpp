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
 * @file             IPmpProtocol.hpp
 * @brief            Header file for IPmpProtocol interface
 * @author           VPlachkov
 ***************************************************************************/

#ifndef IPMPPROTOCOL_HPP_
#define IPMPPROTOCOL_HPP_

#include "framework/components/ProfileManager/PMP/interaction/include/PmpRequestFrame.hpp"
#include "framework/components/ProfileManager/PMP/interaction/include/IPmpCoreClb.hpp"
#include "framework/components/ProfileManager/PMP/interaction/include/IPmpPimClb.hpp"

namespace AXIS
{
   namespace PMP
   {
      /**
       * Interface for PMP Protocol
       */
      class IPmpProtocol
      {
      public:

         /**
          * Function is used to make requests from PMP Core
          */
         virtual PMPFrame * makeCoreRequest(PMPFrame & request) = 0;

         /**
          * Function is used to make requests from PMP PIM
          */
         virtual PMPFrame * makePimRequest(PMPFrame & request) = 0;

         /**
          * Function is used for making responses on another side
          */
         virtual void makeResponse(PMPFrame & response) = 0;

         /**
          * Functions registers PMP Core Callback
          */
         virtual void registerCoreClb(IPmpCoreClb * pCoreClb) = 0;

         /**
          * Function registers PMP PIM Callback
          */
         virtual void registerPimClb(IPmpPimClb * pPimClb) = 0;

      protected:

         /**
          * Destructor
          */
         virtual ~IPmpProtocol() {}
      };
   }
}

#endif /* IPMPPROTOCOL_HPP_ */
