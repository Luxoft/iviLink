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


#ifndef CIPCSOCKET_HPP_
#define CIPCSOCKET_HPP_

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#ifndef ANDROID
#include <sys/un.h>
#else
#include <sys/socket.h>
#include <sys/un.h>
#endif //ANDROID
#include <queue>
#include <vector>

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CRWMutex.hpp"
#include "CThread.hpp"
#include "Logger.hpp"

#include "CIpcError.hpp"
#include "CIpc.hpp"
#include "ipc_common.hpp"

namespace iviLink {
namespace Ipc {

/**
 * Implements request/response transfer state machine.
 * Performs data transfer through unix domain socket.
 *
 * This is sort of pimpl class for CIpc.
 *
 * Creates thread for receiving for socket.
 */
class CIpcSocket : public CThread
{
private:
   static Logger logger;

public:
   /**
    * Constructor.
    * @param ipc
    * @param listen flag specifying how to connect (to connect or to listen).
    */
   CIpcSocket(CIpc& ipc, bool listen);

   /**
    * Destructor.
    * Closes connection and waits until thread working thread is stopped.
    */
   ~CIpcSocket();

   /**
    * Stops the working thread and closes the connection.
    */
   void stopOperations();

   /**
    * Returns the state of connection.
    * @retval true if connected
    * @retval false if connection is not established (can be in listening state)
    */
   bool isConnected() const;


   bool checkThread() const;

   /**
    * Begins the connection.
    * Connection type is specified by parameter of CIpcSocket()
    * @retval IVILINK_NO_ERROR connection established or listen successfully began
    * @retval ERROR_COMMUNICATION connection error
    */
   BaseError connect();

   /**
    * Sets mContinuousListen flag
    * @retval IVILINK_NO_ERROR
    */
   BaseError setContinuousListen();

   /**
    * Stops continuous listen.
    * Other operations and established connections will not be closed.
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_OTHER error in signalSelfpipe()
    */
   BaseError stopContinuousListen();

   /**
    * Sends the request or response packet.
    * Here the packet header will be formed.
    *
    * @param id request id
    * @param isRequest flag specifying the type of packet. @c true if request.
    * @param pPayload pointer to array with payload
    * @param payloadSize size of payload
    * @param pDirId pointer id of client
    *
    * @retval IVILINK_NO_ERROR packet successfully sent
    * @retval ERROR_CONNECTION_LOST no connection established
    * @retval ERROR_COMMUNICATION socket operation error
    * @retval ERROR_UNKNOWN_DIRECTION direction with specified @c dirId
    *    is unknown
    */
   BaseError send(MsgID id, Type messageType,
      UInt8 const* pPayload, UInt32 payloadSize,
      DirectionID const * const pDirId);

private:

   /// States of receiving
   enum RECV_MODE 
   {
      RECV_HEADER,     ///< receiving of header
      RECV_DATA_REQU,  ///< receiving of request
      RECV_DATA_RESP,   ///< receiving of response
      RECV_DATA_TRIG,
   };

   /// Types of signals to working thread send through selfpipe
   enum SELFPIPE_SIGNALS
   {
      SPS_ENDALL,          ///< stop all operations
      SPS_END_CONT_LSN,    ///< stop continuous listen
      SPS_CLI_DIR_CLOSED   ///< client direction has lost connection
   };
private:

   /**
    * Hiding the CThread::start()
    */
   virtual bool start() { return CThread::start(); }

   /**
    * Hiding the CThread::stop()
    */
   virtual bool stop() { return CThread::stop(); }

   /**
    * Preparing the path for socket.
    * The address will be get from #mIpc
    * @return
    */
   bool prepareSocketPath();

   /**
    * Begins the listening for connection.
    * @retval IVILINK_NO_ERROR listen successfully begun
    * @retval ERROR_COMMUNICATION
    */
   BaseError listen();

   /**
    * Accepts the new connection from listening socket.
    * Closes the listening socket and replaces #mSocket
    */
   void acceptConnection();

   /**
    * Closes all sockets
    */
   void closeAllSockets();

   /**
    * Closes client socket (which has direction) and removes the information
    * about it.
    * @param sock socket to be closed
    * @retval true removed socket was last. There are no other working direcions
    * @retval false there are other client sockets
    */
   bool closeClientSocket(int sock);

   /**
    * Closed socket used for listen operation
    * @param needLock if @c true then uses mSocketMutex to lock access
    */
   void closeListenSocket(bool needLock);

   /**
    * Closes socket
    * @param sock
    */
   void closeSocket(int sock);

   /**
    * Setups the pipe used to signal to working thread
    * @retval IVILINK_NO_ERROR pipe is set up
    * @retval ERROR_OTHER pipe is not set up
    */
   BaseError setupSelfpipe();

   /**
    * Closes the pipe
    */
   void destroySelfpipe();

   /**
    * Signals to working thread to stop thread through pipe.
    * @param sig signal
    * @retval IVILINK_NO_ERROR success
    * @retval ERROR_OTHER some error
    */
   BaseError signalSelfpipe(SELFPIPE_SIGNALS sig = SPS_ENDALL);

   /**
    * Called from working thread to receive selfpipe signal
    * @todo proper error handling
    * @return passed signal
    */
   SELFPIPE_SIGNALS recvSelfpipeSignal();

   /**
    * Function for working thread.
    * The heart of receiving state machine cycle.
    */
   virtual void threadFunc();

   /**
    * Selects, what to receive based on #mMode
    * @retval false if connection is lost and receive must be stopped
    * @retval true otherwise
    */
   bool consumeData();

   /**
    * Receives the header, parses it and changes the the #mMode
    * @retval false if connection is lost and receive must be stopped
    * @retval true otherwise
    */
   bool consumeHeader();

   /**
    * Receives the request, passes it to #mIpc and changes #mMode to header
    * receive
    * @retval false if connection is lost and receive must be stopped
    * @retval true otherwise
    */
   bool consumeRequestData();

   /**
    * Receives the response and passes it #mIpc and changes #mMode to header.
    * If response is unknown, reads its content to #mBuffer
    * @retval false if connection is lost and receive must be stopped
    * @retval true otherwise. Always, if response is empty.
    */
   bool consumeResponseData();

   /**
    * Receives the trigger (asynchronous request) and passes it #mIpc and changes #mMode to header.
    * @retval false if connection is lost and receive must be stopped
    * @retval true otherwise
    */
   bool consumeTriggerData();

   /**
    * Receives data to #mpCurrentBuf
    * Changes #mHaveInBuffer and #mLeftToReceive
    *
    * @param size the size of data to be received
    *
    * @retval 1 all data of request/response is retrieved
    * @retval 0 data of request/response is not ready yet
    * @retval -1 got retrieve error
    */
   int recvReqRespData(UInt32& size);

   /**
    * Receives data from socket
    * @param pBuffer pointer to buffer where data will be saved
    * @param[in,out] bufferSize is:
    *     - (in) maximal size of buffer
    *     - (out) actual size of retrieved data. Value is undefined in case of
    *     error.
    * @param peek flag, true if only the size of ready to receive data needed
    *
    * @retval IVILINK_NO_ERROR success, bufferSize holds the actual data size
    * @retval ERROR_CONNECTION_LOST connection is lost
    * @retval ERROR_COMMUNICATION some error
    */
   BaseError receive(UInt8* pBuffer, UInt32& bufferSize, bool peek = false);


   /**
    * Searches socket for given direction id.
    * Access to mSockets is not protected by mutex.
    * 
    * @param pDirId pointer to direction id
    *    If pDirId == NULL, socket of firts direction will be used
    * 
    * @retval socket of given direction
    * @retval -1 if direction is unknown or there is no directions
    */
   int findClientSocket(DirectionID const * const pDirId) const;

   /**
    * Sets the current receive mode and resets the #mLeftToReceive and
    * #mHaveInBuffer
    * @param mode new mode
    * @param leftToReceive size of data of request/response left to receive
    */
   void setMode(RECV_MODE mode, UInt32 leftToReceive);

private:

   /// Reference to CIpc
   CIpc& mIpc;

   /// Flag, server socket
   bool mListen;

   bool mContinuousListen;

   /// Socket file handler
   int mLsnSocket;

   /// Info about client socket
   struct CSockInfo
   {
      int sock;            ///< Socket FD
      DirectionID dirId;   ///< Direction of socket
   };

   typedef std::vector<CSockInfo> tSockets;

   /// Array of client sockets
   tSockets mSockets;

   /// Info about socket from which reading is performed
   CSockInfo mReadingSocket;

   /// Address of unix domain socket
   sockaddr_un mAddress;

   /// Mutex to lock the socket operations
   mutable CRWMutex mSocketMutex;

   /// Mutex to lock send opearations
   CMutex mSendMutex;

   /// Flag to save connection state
   bool mIsConnected;

   /// File descriptors of pipe used to sending signals to working thread
   int mSelfpipe[2];

   /// Bitset used by select()
   fd_set mRFdSet;

   enum {
      BUFFER_SIZE = 4096  ///< Size of internal buffer
   };

   /// Internal buffer used to receive requests or unknown responses
   UInt8 mBuffer[BUFFER_SIZE];

   /// Receiving mode
   RECV_MODE mMode;

   /// Message Id of currently receiving request/response
   MsgID mMsgID;

   /// Flag for unknown response
   bool mUnknownResponse;

   /// Pointer to buffer used for receiving in recvReqRespData
   UInt8* mpCurrentBuf;

   /// Pointer to begin of current buffer
   UInt8* mpCurrentBufBegin;

   /// Size of already received data of request/response
   UInt32 mHaveInBuffer;

   /// Size of data of request/response left to receive
   UInt32 mLeftToReceive;

   std::queue<int> mClosedSocketsQueue;
   CMutex mClosedSocketsQueueMutex;
   
};

}  // namespace Ipc
}  // namespace AXIS

#endif /* CIPCSOCKET_HPP_ */
