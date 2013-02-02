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


#ifndef CONNECTIVITY_MANAGER_HPP
#define CONNECTIVITY_MANAGER_HPP

#include "Types.hpp"
#include "L0Interface.hpp"

namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace HAL
      {
         class CCarrierAdapter;
      }

      namespace L0
      {
         class CQoS;
         class CTransmittedFramesQueue;
         class CTransmitDispatcher;
         class CIncomeDispatcher;

         ///CConnectivityManager class
         /**
          * The CConnectivityManager task is to create and manage HAL layer infrastructure. It receives from Connectivity Agent 
          * notification about available connection and creates objects responsible for data management and transport 
          */

         class  CConnectivityManager
         {

         public:

            // Methods section

            /**
             * Constructor 
             * @param pCarrierAdaptor is the pointer to current carrier adapter which interacts with physical connection created by Connection Agent 
             */
            explicit CConnectivityManager(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdaptor);

            /**
             * Allow to change existing carrier adapter if Connectivity agent created a new instance of adapter(on connection lost for example)
             * warning: Manager should destroy the adapter instance
             * @param pCarrierAdaptor is the pointer to current carrier adapter which interacts with physical connection created by Connection Agent 
             */
            void replaceCarrier(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdaptor);

            /**
             * Interface for Connectivity agent to destroy Manager and carrier adapter
             */
            void destroy();

            L0Interface * getL0Interface ();
            iviLink::ConnectivityAgent::HAL::CCarrierAdapter*  getCarrier();

            void OnDisconnected();

          
         private:

            // Methods section 

            /**
             * Copying constructor
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is constructing on
             */ 
            CConnectivityManager(const CConnectivityManager& classRef );

            /**
             * Assigning operator
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is assiging from
             */     
            CConnectivityManager& operator=(const CConnectivityManager& classRef );

            /**
             * Destructor is private to avoid objects deletion. It is allowed only using destroy() function 
             */ 
            ~CConnectivityManager();

            // Members section

            CQoS*                                               mpQoS;
            CTransmittedFramesQueue*                            mpTransmittedFramesQueue;
            iviLink::ConnectivityAgent::HAL::CCarrierAdapter*      mpCarrierAdapter;
            CIncomeDispatcher*                                  mpIncomeDispatcher;
            CTransmitDispatcher*                                mpTransmitDispatcher;
            L0Interface                                         mL0Interface;
         };
      }
   }
}


#endif
