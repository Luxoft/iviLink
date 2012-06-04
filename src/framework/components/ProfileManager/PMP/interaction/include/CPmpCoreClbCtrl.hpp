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





#ifndef CPMPCORECLBCTRL_HPP_
#define CPMPCORECLBCTRL_HPP_

#include "utils/misc/include/Logger.hpp"
#include "framework/components/ProfileManager/PMP/interaction/include/IPmpProtocol.hpp"
#include "framework/components/ProfileManager/PMP/interaction/include/IPmpCoreClb.hpp"

namespace AXIS
{

   namespace PMP
   {

      /**
       * Class is used for disassembling frames and calling function in PMP Core
       */
      class CPmpCoreClbCtrl
      {
      public:
         /**
          * Constructor
          * @param pProtocol is pointer to object that implements PMP Progocol Interface
          */
         CPmpCoreClbCtrl(IPmpProtocol * pProrocol, IPmpCoreClb * pCoreClb);

         /**
          * Destructor
          */
         virtual ~CPmpCoreClbCtrl();

         /**
          * Functions below parse requests and calls functions in PMP Core
          */
         void onGetAvailableProfileComplements(PMPFrame * pFrame);
         void onReloadProfilesFromRepository (PMPFrame * pFrame);
         void onResetProfilesState(PMPFrame * pFrame);
         void onLock(PMPFrame * pFrame);
         void onUnlock(PMPFrame * pFrame);
         void onUnlockAll(PMPFrame * pFrame);
         void onDisableByClient(PMPFrame * pFrame);
         void onEnableByClient(PMPFrame * pFrame);
         void onEnableByClientAll(PMPFrame * pFrame);

      private:
         IPmpProtocol * mpProtocol; ///< Pointer to PMP Protocol
         IPmpCoreClb * mpCoreClb;   ///< Pointer to PMP Core Callbacks

         static Logger msLogger;    ///< object of logger
      };

   }

}

#endif /* CPMPCORECLBCTRL_HPP_ */
