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


#ifndef CIPC_HPP_
#define CIPC_HPP_

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <map>
#include <string>
#include <tr1/memory>

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "BaseError.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"
#include "Logger.hpp"

#include "CIpcError.hpp"
#include "ipc_common.hpp"

namespace iviLink {
namespace Ipc {

/********************************************************************
 *
 * Forward declarations
 *
 ********************************************************************/
class CIpcSocket;
class ICallbackHandler;

/**
 * CIpc class presents the mechanism for synchronous interprocess communication.
 * It encapsulates all activities for connection establishing, synchronization,
 * sending/receiving.
 * Instead, it offers request-response model.
 *
 * User of this class must instantiate the object, pass the callback handler,
 * call connect() or waitForConnection() to establish connection and use
 * request() method to send data.
 *
 * All incoming requests will be passed as callbacks to callback handler.
 */
class CIpc
{
private:
   static Logger logger;

public:
	// Methods section

   /**
    * Constructor.
    * Connection is not performed in constructor.
    * Client of library must pass @c callbackHandler and guarantee that this
    * handler is alive while CIpc object alive.
    *
    * @param address is the address of component to which connection will be
    *     established
    * @param errorHandler is reference to callback handler
    */
   CIpc(Address const& address, ICallbackHandler& callbackHandler);

   /**
    * Destructor.
    * While destruction, connection is closed (equals to call disconnect()).
    * Calling of class methods from other threads while destruction is running
    * will result in undefined behavior, no thread safety measures will be taken
    */
   ~CIpc();

   /**
    * Called on client side to establish connection.
    * Performs one try to establish connection.
    *
    * @retval IVILINK_NO_ERROR connection is established
    * @retval ERROR_COMMUNICATION error establishing connection
    */
   BaseError connect();

   /**
    * Called on server side to establish connection.
    * This call <b>WILL BLOCK</b> the execution until connection will be
    * established or operation timed out.
    * For setting the timeout value setListenTimeout() is used.
    *
    * @retval IVILINK_NO_ERROR connection established
    * @retval ERROR_LISTEN_TIMEOUT timed out before any connection established
    * @retval ERROR_COMMUNICATION communication error
    */
   BaseError waitForConnection();

   /**
    * Called on server side to establish connection.
    * This call begins the async operation - the listening for connections.
    * The listen timeout (setListenTimeout()) will not be used.
    *
    * @retval IVILINK_NO_ERROR listening successfully begun
    * @retval ERROR_COMMUNICATION communication error
    */
   BaseError beginWaitForConnection();

   /**
    * Called on server side to stop async listening.
    *
    * @retval IVILINK_NO_ERROR listening successfully stopped
    * @retval ERROR_OTHER error of stopping connection
    */
   BaseError endWaitForConnection();

   /**
    * Closes the established connection.
    * <b>DO NOT CALL</b> this from OnRequest() callback handler. This will 
    * result in attempt to join thread to itself - it is wrong.
    * If connection is not established, does nothing.
    * ICallbackHandler::OnConnectionLost() will be called on connection close.
    */
   void disconnect();

   /**
    * Get connection state.
    *
    * @retval true if connection established and ready for transfer.
    * @retval false otherwise.
    */
   bool isConnected() const;

   /**
    * Performs the cheking of the thread. 
    * @retval true if request can be made from this thread (it will lead
    *    to deadlock at least for "request timeout" time)
    * @retval false if request can't be made from this thread, because this is
    *    the thread perfroming onRequest()
    */
   bool checkThread() const;

   /**
    * Returns address of connection's other side.
    *
    * @return address
    */
   Address const& getAddress() const;

   /**
    * Sends synchronous request.
    * This call <b>WILL BLOCK</b> the execution until request will be send and
    * response will be received or operations time out.
    *
    * If request is sent successfully, library begins to wait response. This
    * waiting can expire. Waiting timeout can be set by setResponseTimeout().
    * Library uses provided @c id to retrieve and return proper response for
    * request. User of library must manage the uniqueness of @c id by itself.
    * Uniqueness of @c id is meaningful in case when requests are sent from 
    * different threads.
    *
    * Thread safety: simultaneous calls from different threads will not
    * interfere each other.
    * @a Currently, only one thread will perform request,
    * Other threads will block until operation end or blocking time expiring.
    * Blocking timeout can be set by setRequestTimeout().
    * This can be changed in the future.
    *
    * If disconnect() is called from other thread while request() is performed,
    * result can vary:
    *   - if request is not sent yet (because of thread was blocked by call to
    *   request() from other threads, for example), it will return
    *   ERROR_CONNECTION_LOST
    *   - if request is sent, it will wait until response timeout and fail with
    *   ERROR_RESPONSE_TIMEOUT error. This can be changed later.
    * If this is possible, better to avoid such code execution.
    *
    * @param id is request ID number, must be unique
    * @param pPayload is pointer to array with request data
    * @param payloadSize is size of request data
    * @param pResponseBuffer is pointer to buffer where response data
    *     will be held
    * @param[in,out] bufferSize is:
    *     - (in) maximal size of buffer for response data;
    *     - (out) size of actual received size. In case of error the returned
    *     value is undefined.
    * @param pDirId is pointer to client ID to which request must be sent.
    *     If pDirId == NULL, request will be sent to the first client.
    *
    * @retval IVILINK_NO_ERROR - request done successfully, response in passed buffer
    * @retval ERROR_INVALID_PARAMS invalid parameters
    * @retval ERROR_REQUEST_TIMEOUT request timeout (see above)
    * @retval ERROR_RESPONSE_TIMEOUT response timeout
    * @retval ERROR_SMALL_BUFFER response buffer is to small
    * @retval ERROR_COMMUNICATION communication error
    * @retval ERROR_CONNECTION_LOST connection lost during request
    * @retval ERROR_UNKNOWN_DIRECTION client with specified @c pDirId is unknown
    * @retval ERROR_WRONG_THREAD attempt to make call somewhere from thread,
    *     which performing onRequest()
    * @retval ERROR_OTHER some other error
    *
    * Possible improvements:
    * @todo allow to send big requests
    * @todo allow to send requests from multiple threads simultaniously.
    * @todo stop waiting for response if connection is lost in this time.
    */
   BaseError request(MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      UInt8* const pResponseBuffer, UInt32& bufferSize, 
      DirectionID const* const pDirId = NULL);

   /**
    * Asyncronous request: will send pPayload[payloadSize] and exit
    * - no wait for response unlike the 'request' method.
    * On the other side OnAsyncRequest will be invoked.
    */
   BaseError asyncRequest(MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      DirectionID const* const pDirId = NULL);

   /**
    * Getter for request timeout
    * @return current request timeout value in milliseconds
    */
   time_t getRequestTimeout() const;

   /**
    * Setter for request timeout.
    * Changing the value while other threads are waiting for request will not
    * change their waiting time.
    * @param timeout new value for request timeout in milliseconds
    */
   void setRequestTimeout(time_t timeout);

   /**
    * Getter for response timeout
    * @return current response timeout value in milliseconds
    */
   time_t getResponseTimeout() const;

   /**
    * Setter for response timeout.
    * Changing the value while other threads are waiting for response will not
    * change their waiting time.
    * @param timeout new value for request timeout in milliseconds
    */
   void setResponseTimeout(time_t timeout);

   /**
    * Getter for listen timeout.
    * @return current listen timeout value in milliseconds
    */
   time_t getListenTimeout() const;

   /**
    * Setter for listen timeout.
    * Changing the value while other thread is waiting for new connection will
    * not change its waiting time.
    * @param timeout new value for listen timeout in milliseconds
    */
   void setListenTimeout(time_t timeout);

private:
   // Types section

   /**
    * Type for proper sharing of semaphore between CIpc and CIpcSocket.
    */
   typedef std::tr1::shared_ptr<CSignalSemaphore> tSharedSem;

   /**
    * Holds the data about response which will be retrieved.
    */
   struct RequestData
   {
      UInt8* pResponseBuffer; ///< pointer to buffer for response
      UInt32 bufferSize;      ///< size of buffer and size of retrieved response
      tSharedSem recvSem;     ///< semaphore to signal about retrieved response
      DirectionID mDirID;
   };

   /**
    * Container to hold the data about responses which will be retrieved and
    * to share it with the CIpcSocket class.
    */
   class CRequestDataContainer
   {
   public:
      /**
       * Saves data about response which will be retrieved.
       * @param id is the unique request id
       * @param pResponseBuffer is pointer to buffer for response
       * @param bufferSize is the size of buffer
       * @param recvSem is the semaphor for signaling about response receiving
       * @param pDirId id of client the request sender
       * @retval true in case of success
       * @retval false in case when id is already exist in container
       */
      bool insert(MsgID id, UInt8* const pResponseBuffer, UInt32 bufferSize, tSharedSem recvSem, const DirectionID * pDirId);

      /**
       * Erases requests for the specified direction and signals about response receiving
       * (since the requests are erased, requesters will get an error retrieving responses
       * and will report the CONNECTION_LOST error)
       * @param dirId id of client the request sender
       */
      void invalidateRequestsForDirection(DirectionID dirId);

      /**
       * Retrieves the data form container.
       * @param id is request id
       * @param data is reference to RequestData object where data retrieved
       *     data will be saved
       * @retval true in case of success. @c data holds the data about response
       * @retval false in case when @c id is not found
       */
      bool getData(MsgID id, RequestData& data) const;

      /**
       * Checks if the record with @c id is contained in container.
       * @param id is request id
       * @retval true in case if @c id is in container
       * @retval false otherwise
       */
      bool haveData(MsgID id) const;

      /**
       * Removes data with @c id from container
       * @param id is the request id
       */
      void erase(MsgID id);

      /**
       * Retrieves the data from container and locks it.
       * Used to change the data in container.
       * Container is unlocked by unlockData(). All other calls will be blocked.
       * @param id is the request id
       * @param[out] data is pointer to data in container. This pointer can be
       *     used to change the data in container.
       * @retval true in case if @c id is in container
       * @retval false otherwise
       */
      bool lockData(MsgID id, RequestData*& data);

      /**
       * Unlocks the container locked by lockData().
       * @param id request id
       */
      void unlockData(MsgID id);

   private:

      /// Map-like container
      typedef std::map<MsgID, RequestData> tRequestDataMap;

      tRequestDataMap mRequestDataMap;    ///< internal container
      mutable CMutex mRequestDataMapMutex;///< mutex to lock container
   };

private:
	// Methods section

   /**
    * Unimplemented copy constructor.
    * @param ref
    */
   CIpc(CIpc const& ref);
   /**
    * Unimplemented operator=.
    * @param ref
    */
   CIpc& operator=(CIpc const& ref);

   /**
    * Called by CIpcSocket on incoming request.
    *
    * @param id request id
    * @param pPayload pointer to buffer with request payload
    * @param payloadSize size of payload
    * @param dirId id of client the request sender
    */
   void incomingRequest(MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize,
      DirectionID dirId);


   void incomingAsyncRequest(MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize,
      DirectionID dirId);

   /**
    * Destroys the CIpc::mSocket
    */
   void destroySocket();

   /**
    * Called by CIpcSocket on listen success, when connection is established.
    *
    * @param dirId id of new direction
    */
   void onListenSuccess(DirectionID dirId);

   /**
    * Called by CIpcSocket on connect success, when connection is established.
    *
    * @param dirId id of new direction
    */
   void onConnectSuccess(DirectionID dirId);

   /**
    * Called by CIpcSocekt on connection lost or when connection is closed.
    *
    * @param dirId id of lost direction
    */
   void onConnectionLost(DirectionID dirId);

private:
	// Fields section

   /// Pointer to callback handler passed by client of CIpc
   ICallbackHandler* const mpClientHandler;

   /// Address passed by client of CIpc
   Address const mAddress;

   friend class CIpcSocket;
   /// Internal data transfer module
   CIpcSocket* mSocket;

   /**
    * Container used to hold the data about responses which will be received.
    * Data from container is removed when response waiting is timed out.
    * CIpcSocket uses this container to pass the received response.
    */
   CRequestDataContainer mRequestDataContainer;

   /**
    * Mutex for critical section in request().
    * It is used to wait for request timeout and to ensure that only one
    * request is send on one moment.
    */
   CMutex mRequestMutex;

   /**
    * Semaphore used to wait for listen timeout.
    */
   CSignalSemaphore mListenSem;

   time_t mRequestTimeout;   ///< current value for request timeout
   time_t mResponseTimeout;  ///< current value for response timeout
   time_t mListenTimeout;    ///< current value for listen timeout

   enum {
      RESP_BUFFER_SIZE = 4096 ///< size of buffer for response
   };

   /**
    * Buffer used to hold response for sending. This buffer is filled by
    * library client in ICallbackHandler::OnRequest() callback.
    */
   UInt8 mRespBuffer[RESP_BUFFER_SIZE];

   /// Mutex used to lock CIpc::mRespBuffer
   CMutex mRespBufferMutex;

};

}  // namespace Ipc
}  // namespace AXIS

#endif /* CIPC_HPP_ */
