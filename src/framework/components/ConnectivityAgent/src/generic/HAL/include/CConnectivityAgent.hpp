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
 * (c) copyright     2011
 * Company           LUXOFT
 * @file             CConnectivityAgent.hpp
 * @brief            The CConnectivityAgent class header file
 * @author           PIlin
 ***************************************************************************/

#ifndef CCONNECTIVITYAGENT_HPP_
#define CCONNECTIVITYAGENT_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

#include "utils/threads/include/CThread.hpp"
#include "utils/threads/include/CMutex.hpp"
#include "utils/threads/include/CRWMutex.hpp"
#include "utils/threads/include/CSignalSemaphore.hpp"
#include "utils/misc/include/Types.hpp"

/********************************************************************
 *
 * Forward declarations
 *
 ********************************************************************/
class CCarrierAdapter;
class CConnectivityManager;
class HALInterface;

class CDummyConnectionFinder;
class CTcpAutoConnectionFinder;

/// CConnectivityAgent class
/**
 * Purpose of ConnectivityAgent is to manage of process of finding possibility of new connections,
 * their establishing and maintaining.
 */
class CConnectivityAgent : public CThread
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
   HALInterface* getHALInterface();
   CCarrierAdapter* getCurrentCarrierAdapter();
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
    * @return true on success
    */
   bool initAdapterHandshake(CCarrierAdapter* pAdapter);

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

   CRWMutex mManagerMutex;

   /// Current connectivity manager
   CConnectivityManager* mpManager;

   /// Flag indicates that connection on this carrier is established and we don't need to find new
   /// connections through this carrier
   bool mConnectionEstablished[eLastCarrier];

   bool mIsServer;

   /// Semaphore used by connection finders to signal about new connection
   CSignalSemaphore mSigSem;

   CDummyConnectionFinder* mpDummyConnectionFinder;
   CTcpAutoConnectionFinder* mpTcpAutoConnectionFinder;

};

#endif /* CCONNECTIVITYAGENT_HPP_ */
