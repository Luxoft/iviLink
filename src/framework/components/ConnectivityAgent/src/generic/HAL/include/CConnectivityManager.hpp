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





#ifndef CONNECTIVITY_MANAGER_HPP
#define CONNECTIVITY_MANAGER_HPP

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/


/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/ 
#include "utils/misc/include/Types.hpp"

#include "CQoS.hpp"
#include "CTransmittedFramesQueue.hpp"
#include "CTransmitDispatcher.hpp"
#include "CIncomeDispatcher.hpp"
#include "HALInterface.hpp"


class CIncomeDispatcher;
class CTransmitDispatcher;
class CCarrierAdapter;

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
   explicit CConnectivityManager(CCarrierAdapter* pCarrierAdaptor);

   /**
    * Allow to change existing carrier adapter if Connectivity agent created a new instance of adapter(on connection lost for example)
    * warning: Manager should destroy the adapter instance
    * @param pCarrierAdaptor is the pointer to current carrier adapter which interacts with physical connection created by Connection Agent 
    */
   void replaceCarrier(CCarrierAdapter* pCarrierAdaptor);

   /**
    * Interface for Connectivity agent to destroy Manager and carrier adapter
    */
   void destroy();

   HALInterface * getHALInterface ();
   CCarrierAdapter*  getCarrier();

 
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

   CQoS                                         mQoS;
   CTransmittedFramesQueue                      mTransmittedFramesQueue;
   CCarrierAdapter*                             mpCarrierAdapter;
   CIncomeDispatcher                            mIncomeDispatcher;
   CTransmitDispatcher                          mTransmitDispatcher;
   HALInterface                                 mHALInterface;
};
#endif
