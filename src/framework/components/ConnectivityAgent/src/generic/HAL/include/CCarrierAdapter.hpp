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
 * File              CCarrierAdapter.cpp
 * Description
 * @Author           PIlin
 ***************************************************************************/

#ifndef CCARRIERADAPTER_HPP_
#define CCARRIERADAPTER_HPP_

#include "utils/misc/include/Types.hpp"
#include "utils/threads/include/CMutex.hpp"

class Frame;
class IFrameReceiver;

class CCarrierAdapter
{
private:
   static Logger logger;

   friend void adapterTesting();
public:
   CCarrierAdapter();
   virtual ~CCarrierAdapter();

   /**
    * Is used for sending frames to connected device
    * @param pFrame is pointer on frame to send
    * @return error code if it can't send frame
    */
   ERROR_CODE sendFrame(Frame* pFrame);

   /**
    * @param receiver sets mpFrameReceiver value
    */
   ERROR_CODE registerFrameReceiver(IFrameReceiver& receiver);

   /**
    * This function will make handshake and test basic low-level connectivity.
    * @return result of handshake.
    */
   virtual ERROR_CODE makeHandshake() = 0;

   /**
    * This function is used by CConnectivityAgent to determine broken connections.
    * @return true if connection is broken.
    */
   virtual bool isBroken() const = 0;

   /**
    * Returns address of opposite side of connection.
    * @return pointer to c-string containing the address of other side
    * of connection or NULL if there is no such address.
    */
   virtual const char* getRemoteAddress() const;

   /**
    * Returns address of our side of connection. This address is known to
    * opposite side, i.e. whis address will be returned by getRemoteAddress()
    * on the opposite side.
    *
    * @returns pointer to c-string, containing address of local side or
    * NULL if there is no such address.
    */
   virtual const char* getLocalAddress() const;

   void resendACK(UInt32 channel_id, UInt32 frame_number);

protected:

   /**
    * Used to send prepared data array through carrier.
    * @param pArray - pointer to data array
    * @param size - size of data array
    * @return operation error code
    */
   virtual ERROR_CODE sendPrepearedArray(const UInt8* pArray, UInt32 size) = 0;

   /**
    * Used to receive raw data array from carrier. This is blocking call, it must receive all requested
    * data.
    * @param pArray - pointer to data array
    * @param size - size of requested data
    * @return operation error code
    */
   virtual ERROR_CODE receiveRawArray(UInt8* pArray, UInt32 & size) = 0;

   /**
    * @todo: I think, this method will be called by carrier api somehow to signal
    * that we got new data and we need to receive them.
    * @param frame is reference to buffer frame
    * @param retry if true then, on last receiveFrame call, frame header was got, but data was timeouted, so we need next attempt to get data.
    * @return
    */
   ERROR_CODE receiveFrame(Frame& frame, bool retry);

   /**
    * This function is used to send ACK 
    * @return ERR_OK
    */
   ERROR_CODE sendACK(Frame &frame);

private:

   // This delegate will get received frame and process it.
   IFrameReceiver* mpFrameReceiver;

   CMutex mFrameReceiverLock;

   // Converts byte order of frame's fields before sending and after receiving
   // Data will be always transmitted using big-endian byte order
   class CFrameConverter
   {
   public:
      static void frameToArray(const Frame* pFrame, UInt8* pArray);
      static void arrayToFrame(const UInt8* pArray, Frame* pFrame);
   };
};

#endif /* CCARRIERADAPTER_HPP_ */
