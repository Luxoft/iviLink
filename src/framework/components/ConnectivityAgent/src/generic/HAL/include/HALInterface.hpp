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




#ifndef HALINTERFACE_HPP
#define HALINTERFACE_HPP
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
#include "utils/misc/include/Logger.hpp"

class IBufferProducer;
class IBufferConsumer;
class CTransmitDispatcher;
class CIncomeDispatcher;
class CQoS;

 ///HALInterface class
/**
 * The HALInterface implements interface to HAL layer that should be used by upper layers from Axis Protocol Stack.
 */
 
class  HALInterface
{
private:
   static Logger logger;

public:
   // Methods section

   /**
    * Constructor
    * @param pTransmitDispatcher is the pointer to current CTransmitDispatcher
    * @param pIncomeDispatcher is the pointer to current CIncomeDispatcher
    * @param pQoS is the pointer to current CQoS
    */
   HALInterface(CTransmitDispatcher *pTransmitDispatcher, CIncomeDispatcher* pIncomeDispatcher, CQoS* pQoS);

   /**
    * Destructor
    */    
   ~HALInterface();

   /**
    * Open the outgoing  channel with fixed number and priority. It delegates CTransmitDispatcher to open the channel and
    * creates the QoS entry for given channel
    * @param priority is the priority of a channel
    * @param channel_id is the channel number
    * @param bufferProducer is the pionter L0 source representaion i.e. SourceAgent to exchange the freed buffers
    * @return ERR_OK
    */
   ERROR_CODE openTransmissionChannel(const UInt32 channel_id, const UInt8 priority, IBufferProducer& bufferProducer,
                                       UInt64 upper_threshold, UInt64 lower_threshold);
    /**
    * Open theincoming channel with fixed number and priority. It delegates CIncomeDispatcher to open the channel
    * @param priority is the priority of a channel
    * @param channel_id is the channel number
    * @param bufferConsumer is the connection with L0 target representaion i.e. TargetAgent to exchange the freed buffers
    * @return ERR_OK
    */
   ERROR_CODE openIncomingChannel(const UInt32 channel_id, IBufferConsumer& bufferConsumer);

   /**
    * Close the outgoing channel by its number. Delegates closing to CTransmitDispatcher
    * @param channel_id is the channel number
    */   
   void closeTransmissionChannel(const UInt32 channel_id);

   /**
    * Close incoming channel by its number. Delegates closing to CIncomeDispatcher
    * @param channel_id is the channel number
    */   
   void closeIncomingChannel(const UInt32 channel_id);

   /**
    * Allows to clear buffers for given channel_id
    * @param channel_id is the channel number
    */     
   void clearBuffers(const UInt32 channel_id);

   /**
    * Gets average time for frame pass through given channel number
    * @param channel_id is the channel number
    * @return average time 
    */
   UInt64 getQoSReport(const UInt32 channel_id);

   private:
   // Methods section 

   /**
    * Copying constructor
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is constructing on
    */ 
   HALInterface(const HALInterface&);

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */
   HALInterface& operator=(const HALInterface&);

   private:

   // Members section
   CTransmitDispatcher*          mpTransmitDispatcher;
   CIncomeDispatcher *           mpIncomeDispatcher;
   CQoS*                         mpQoS;
};

#endif
