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


#ifndef CCARRIERADAPTER_HPP_
#define CCARRIERADAPTER_HPP_

#include "Types.hpp"
#include "CMutex.hpp"
#include "Frame.hpp"
#include "EGenderType.hpp"
#include "Logger.hpp"
#include "ConnectivityAgentError.hpp"


namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace L0
      {
         class IFrameReceiver;
      }

      namespace HAL
      {
        class Frame;

        class CCarrierAdapter
        {
        private:
           static Logger logger;

           friend void adapterTesting();
        public:
           CCarrierAdapter();
           virtual ~CCarrierAdapter();

           /**
            * Is used for sending frames to the connected device
            * @param pFrame is pointer on frame to send
            * @return errors with one of the following codes: 
            * ERROR_INCONSISTENT_DATA if pFrame is null or the frame is malformed (its result size is zero), 
            * ERROR_BROKEN_PIPE if it can't send frame due to connection problems
            * IVILINK_NO_ERROR in case of success
            */
           ConnectivityAgentError sendFrame(iviLink::ConnectivityAgent::HAL::Frame* pFrame);

           /**
            * Sets mpFrameReceiver value.
            * @param receiver 
            */
           void registerFrameReceiver(iviLink::ConnectivityAgent::L0::IFrameReceiver& receiver);

           /**
            * This function will make handshake and test basic low-level connectivity.
            * @return error with one of the following codes:
            * ERROR_HANDSHAKE_FAILED if something went wrong during handshake
            * IVILINK_NO_ERROR in case of success
            */
           virtual ConnectivityAgentError makeHandshake() = 0;

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

           /**
            * Returns string representation of carrier type.
            * String must be statically allocated and used to determine type of
            * address that will be returned by getRemoteAddress() and getLocalAddress().
            * @return type string, not NULL
            */
           virtual const char* getTypeName() const = 0;

           /**
            * Returns current gender of adapter - client- or server-side.
            * 
            * For connectionless adapters (if there will be any someday) gender
            * can be decided any, but it must be differ on different sides.
            * In this case, good time to decide gender will be handshake.
            *
            * @retval eServerGender this is server side.
            * @retval eClientGender this is client side.
            * @retval eAnyGender side is not decided yet - probably connection 
            *   is not established or handshake is not performed.
            */
           EGenderType getGender() const;

        protected:

           /**
            * Used to send prepared data array through carrier.
            * @param pArray - pointer to data array
            * @param size - size of data array
            * @return error with one of the following codes:
            * ERROR_INCONSISTENT_DATA if pArray is null or size is 0
            * ERROR_BROKEN_PIPE if data could not be sent because of connection problems
            * IVILINK_NO_ERROR in case of success
            */
           virtual ConnectivityAgentError sendPreparedArray(const UInt8* pArray, const UInt32 size) = 0;

           /**
            * Used to receive raw data array from carrier. 
            * @param pArray - pointer to data array
            * @param[in, out] size - size of data array and amount of actually
            *   received data
            * @return error with one of the following codes:
            * ERROR_BROKEN_PIPE if data could not be sent because of connection problems
            * IVILINK_NO_ERROR in case of success
            */
           virtual ConnectivityAgentError receiveRawArray(UInt8* pArray, UInt32 & size) = 0;

           /**
            * Called by specific carrier adapter to show that there is data 
            * available.
            * Method will get available data, process it and, if frame is 
            * received completely, pass it to registered frame receiver.
            * If frame is incomplete, it will be saved until next call to 
            * haveData().
            */
           void haveData();

           /**
            * Passes frame to registered frame receiver and sends ACK if needed
            * @param frame
            * @return result from frame receiver
            */
           ConnectivityAgentError processFrame(iviLink::ConnectivityAgent::HAL::Frame & frame);

           /**
            * This function is used to send ACK 
            * @return error with one of the following codes:
            * ERROR_BROKEN_PIPE if data could not be sent because of connection problems
            * IVILINK_NO_ERROR in case of success
            */
           ConnectivityAgentError sendACK(iviLink::ConnectivityAgent::HAL::Frame &frame);

      protected:
         // members section

         /// Gender type of the adapter. Must be decided before makeHandshake() 
         /// call is done. 
         /// See getGender() documentation.
         EGenderType mGenderType;


      private:
         UInt32 mFilledSize;

         Frame mFrame;


           // This delegate will get received frame and process it.
           iviLink::ConnectivityAgent::L0::IFrameReceiver* mpFrameReceiver;

           CMutex mFrameReceiverLock;

           
           // Converts byte order of frame's fields before sending and after receiving
           // Data will be always transmitted using big-endian byte order
           class CFrameConverter
           {
           public:
              static void frameToArray(const iviLink::ConnectivityAgent::HAL::Frame* pFrame, UInt8* pArray);
              static void arrayToFrame(const UInt8* pArray, iviLink::ConnectivityAgent::HAL::Frame* pFrame);
           };
        };
     }
  }
}

#endif /* CCARRIERADAPTER_HPP_ */
