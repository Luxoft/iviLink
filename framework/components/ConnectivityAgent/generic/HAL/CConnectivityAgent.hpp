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


#ifndef CCONNECTIVITYAGENT_HPP_
#define CCONNECTIVITYAGENT_HPP_

#include <vector>

#include "CThread.hpp"
#include "CMutex.hpp"
#include "CRWMutex.hpp"
#include "CSignalSemaphore.hpp"
#include "Types.hpp"
#include "ConnectivityAgentError.hpp"

#include "CCarrierAdapter.hpp"

#include "IFoundConnectionHandler.hpp"
#include "CreatePlatformSpecificFinders.hpp"
#include "EGenderType.hpp"

namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace L0
      {
         class CConnectivityManager;
         class L0Interface;
      }

      namespace HAL
      {
         /// CConnectivityAgent class
         /**
          * Purpose of ConnectivityAgent is to manage of process of finding possibility of new connections,
          * their establishing and maintaining.
          */
         class CConnectivityAgent :
            public CThread,
            public IFoundConnectionHandler
         {
         private:
            static Logger logger;

         public:

            /**
             * Constructor
             * @param isServer - flag is true if application is on server-side
             */
            CConnectivityAgent(bool isServer);

            /**
             * Destructor
             */
            virtual ~CConnectivityAgent();

            /**
             * Method running in separate thread
             */
            virtual void threadFunc();

            /**
             * This call is used by CConnectivityManager to inform agent about connection problems/
             * Agent will inform HALController about lost connection and begin recovering connection.
             * If connection recover failed, disconnect() method called.
             */
            void onConnectionProblem(ECarrierType carrierType);

            /**
             * Returns pointer to HALInterface from created CConnectionManager.
             * @return pointer to HALInterface or NULL
             */
            iviLink::ConnectivityAgent::L0::L0Interface* getL0Interface();
            CCarrierAdapter* getCurrentCarrierAdapter();

            EGenderType getCurrentGender() const;

         public:
            // from IFoundConnectionHandler

            virtual void connectionFound(CConnectionFinder* pFinder);

         private:
            // Methods section

            /**
             * Method tries to establish new connections.
             */
            void checkNewCarriers();


            /**
             * Establishes new dummy connection.
             * @return adapter instance if connection successfully established or NULL.
             */
            CCarrierAdapter* initNewDummyConnection();

            /**
             * Establishes new Tcp auto connection.
             * @return  adapter instance if connection successfully established or NULL.
             */
            CCarrierAdapter* initNewTcpAutoConnection();

            /**
             * Helper function. Makes handshake through passed adapter, and, in the case of success, creates
             * object of CConnectionManager class.
             * @param pAdapter - pointer to connected adapter
             * @return IVILINK_NO_ERROR in case of success
             */
            ConnectivityAgentError initAdapterHandshake(CCarrierAdapter* pAdapter);

            /**
             * Makes a try to recover broken connection.
             * @param  carrierType - carrier type
             * @return new carrier adapter or NULL if recover is failed
             */
            CCarrierAdapter* tryToRecover(ECarrierType carrierType);

            /**
             * Informs HALController about disconnect and destroys CConnectivityManager.
             */
            void disconnect();


            /**
             * Copying constructor.
             * Unimplemented to avoid misusing.
             * @param classRef The reference to the object the class is constructing on.
             */
            CConnectivityAgent(const CConnectivityAgent& classRef);

            /**
             * Assigning operator.
             * Unimplemented to avoid misusing.
             * @param classRef The reference to the object the class is assigning from.
             */
            CConnectivityAgent& operator=(const CConnectivityAgent& classRef);

         private:
            // Members section

            CMutex mMainFuncMutex;

            bool mConnectionEstablished;

            CRWMutex mManagerMutex;

            /// Current connectivity manager
            iviLink::ConnectivityAgent::L0::CConnectivityManager* mpManager;

            EGenderType mGenderType;

            tFinders mFinders;

         };
      }
   }
}

#endif /* CCONNECTIVITYAGENT_HPP_ */
