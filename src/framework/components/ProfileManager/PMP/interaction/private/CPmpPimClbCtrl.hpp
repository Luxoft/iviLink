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











#ifndef CPMPPIMCLBCTRL_HPP_
#define CPMPPIMCLBCTRL_HPP_

#include "IPmpProtocol.hpp"
#include "IPmpPimClb.hpp"

namespace iviLink
{

   namespace PMP
   {

      /**
       * Class is controller PMP PIM Callbacks
       */
      class CPmpPimClbCtrl
      {
      public:

         /**
          * Constructor
          * @param pProtocol is pointer to object that implements PMP Protocol
          * @param pPimClb is pointer to object that implements PIM Callbacks
          */
         CPmpPimClbCtrl(IPmpProtocol * pProrocol, IPmpPimClb * pPimClb);

         /**
          * Destructor
          */
         virtual ~CPmpPimClbCtrl();

         /**
          * functions below disassembles frames and calls functions in PMP PIM Callback class
          */
         void onCreateProfile(PMPFrame * pFrame);
         void onProfileDied(PMPFrame * pFrame);

      private:
         IPmpProtocol * mpProtocol; ///< Pointer to PMP Protocol class
         IPmpPimClb * mpPimClb;     ///< Pointer to PIM callback classghb
      };

   }

}

#endif /* CPMPPIMCLBCTRL_HPP_ */
