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
* @file             CChannelAllocator.hpp
* @brief            The CChannelAllocator class header file
* @author           KSkidanov
***************************************************************************/
#ifndef CCHANNEL_ALLOCATOR_HPP
#define CCHANNEL_ALLOCATOR_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
 #include <vector>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/misc/include/Types.hpp"
#include "L0InterfaceStub.hpp"
#include "utils/threads/include/CMutex.hpp"
#include "utils/threads/include/CMutex.hpp"
#include "framework/components/ConnectivityAgent/src/generic/common/include/API.hpp"


class HALInterface;

///CChannelAllocator class
/**
 * The CChannelAllocator implemented as Singleton. It's task is to manage the channels allocation and deallocation by requests
 * from upper layers of AXIS Protocol Stack. The only exclusion is creating of service channel which is initiated by HAL when
 * physical connection is established
 */
class CChannelAllocator
{
private:
   static Logger logger;

public:

   // Methods section
   /**
    * Interface function to provide sigleton behavior
    * @return pointer to existing instance of singleton and creates it if not exists
    */
   static CChannelAllocator* getInstance();

   /**
    * Interface function to destroy the singleton instance
    */
   void deleteInstance();

   /**
    * Sets current HALInterface pointer to CChannelAllocator
    * @param pHALInterface current HALInterface pointer
    */
   void setHALInterface(HALInterface *pHALInterface);

   /**
    * Callback function called by HAL ConnectivityManager to create service channel
    */
   void OnConnected();

   /**
    * Callback function called by HAL ConnectivityManager on lost connection
    */
   void OnDisconnected();

   /**
    * Interface for L1 to allocate outgoing channels
    * @return ERR_OK
    */
   ERROR_CODE allocateChannel(TChannelPriority prio, UInt32 channel_id, L0InterfaceStub::tChannelInfo& channelInfo);

   /**
    * Interface for L1 to deallocate outgoing channels
    * @return ERR_OK
    */
   ERROR_CODE deallocateChannel(UInt32 channel_id);


   /**
    * Interface for L1 to get QOS statistic for channel
    * @return ERR_OK
    */
   UInt64 getQoSReportForChannel(const UInt32 channel_id);

   void narrowBandwidth(UInt32 channel_id);
   void higherBandwidth(UInt32 channel_id);

private:

   // Methods section
   /**
    * Constructor is private to provide sigleton behavior
    */
   CChannelAllocator();

   /**
    * Destructor
    */
   ~CChannelAllocator();

   /**
    * Copying constructor
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is constructing on
    */
   CChannelAllocator(const CChannelAllocator& );

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */
   CChannelAllocator& operator=(const CChannelAllocator& );

private:

   // Members section

   static CChannelAllocator*                 mSelf;
   HALInterface*                             mpHALInterface;
   static CMutex                             mSingletonMutex;
};

#endif
