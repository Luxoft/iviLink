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


#ifndef ICALLBACKHANDLER_HPP_
#define ICALLBACKHANDLER_HPP_

#include "Types.hpp"
#include "ipc_common.hpp"

namespace iviLink {
namespace Ipc {

/**
 * Interface for application's callback handler.
 * Application must implement this interface and pass the handler to CIpc
 * object.
 */

class ICallbackHandler
{
public:

   /**
    * Called on connection establishing.
    * It will be called in case, when:
    * 1) outgoing connection is established through CIpc::connect()
    * 2) incoming connection is established through CIpc::waitForConnection() 
    * 
    * @param dirId id of direction
    */
   virtual void OnConnection(DirectionID dirId) = 0;


   /**
    * Called on connection lost.
    * It will be called in case, when:
    * 1) connection is closed from other side;
    * 2) connection is closed from this side;
    * 3) inner interconnection code met fatal error and unable to continue
    * the send/receive processing
    *
    * @param dirId id of direction
    */
   virtual void OnConnectionLost(DirectionID dirId) = 0;

   /**
    * Called on incoming request.
    * Application must implement this method to be able to respond to the
    * requests.
    * Response can be empty. In this case, @c bufferSize must be set to 0.
    *
    * @param id is request ID number
    * @param pPayload is pointer to array with request data
    * @param payloadSize is size of request data array
    * @param pResponseBuffer is pointer to buffer, where response
    * must be placed. Buffer for response is allocated by IPC
    * library.
    * @param[in,out] bufferSize is:
    *   - (in) full size of allocated for response
    *   - (out) size of response data written to buffer
    * @param dirId id of direction the request sender
    *
    * @todo possibility to increase buffer for response. Currently, its size
    * is 4096 bytes (sets by constant @c CSocket::RESP_BUFFER_SIZE)
    */
   virtual void OnRequest(iviLink::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      UInt8* const pResponseBuffer, UInt32& bufferSize,
      DirectionID dirId) = 0;

   virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      DirectionID dirId) = 0;

protected:

   virtual ~ICallbackHandler() {}
};

}  // namespace Ipc
}  // namespace AXIS

#endif /* ICALLBACKHANDLER_HPP_ */
