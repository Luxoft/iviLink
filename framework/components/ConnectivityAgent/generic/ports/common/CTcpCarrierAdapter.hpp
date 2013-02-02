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


#ifndef CTCPCARRIERADAPTER_HPP_
#define CTCPCARRIERADAPTER_HPP_

#include <ctime>

#include "Logger.hpp"
#include "ConnectivityAgentError.hpp"
#include "Types.hpp"
#include "CThread.hpp"
#include "CRWMutex.hpp"
#include "CCondVar.hpp"

#include "CCarrierAdapter.hpp"
#include "Frame.hpp"
#include "ConnectionInfo.hpp"


struct addrinfo;

namespace iviLink {
namespace ConnectivityAgent {
namespace HAL {

/// CTcpCarrierAdapter class
/**
 * Class is used to manage connection by TCP/IP socket.
 * It encapsulates connection establishing and data transfer.
 *
 * Connection is established when newly created object of class is not "broken" 
 * (isBroken()).
 *
 * CTcpCarrierAdapter creates separate thread and uses select() to accept new
 * incoming connections or wait for incoming data.
 *
 */
class CTcpCarrierAdapter   : public CCarrierAdapter
                           , public CThread
{
private:
   static Logger logger;

   /**
    * Friend is used because this class has private constructor
    */
   friend class CTcpAutoConnectionFinder;
   friend class TcpMobileConnectionFinder;

public:
   // Methods section

   virtual ~CTcpCarrierAdapter();

   /**
    * Establishes new connection.
    * The behavior of function differs when socket is server or client:
    * - if socket is client side, ::connect() is used. If connection was refused,
    *   method fails.
    * - if socket is server side, ::listen() is used. If listen is successfully
    *   began, method finishes successfully, but real connection is not 
    *   established yet and object is in "broken" state until incoming 
    *   connection will be accepted.
    *
    * If method finishes successfully, new working thread will be started.
    * Also, method finishes successfully, if object already have connection (not
    * in "broken" state). In this case, new thread is not created.
    *
    * @retval ERR_OK success
    * @retval ERR_FAIL some connection error
    */
   ConnectivityAgentError connect();

   /**
    * Closes connection (if any), stops listening for incomming connections,
    * finishes working thread.
    * Method is non-blocking, so all disconnecting can take place later.
    */
   void disconnect();

public:
   // from CCarrierAdapter
   virtual bool isBroken() const;
   virtual ConnectivityAgentError makeHandshake();
   virtual const char* getRemoteAddress() const;
   virtual const char* getLocalAddress() const;

protected:
   // Methods section
   // from CCarrierAdapter

   virtual ConnectivityAgentError sendPreparedArray(const UInt8* pArray, const UInt32 size);
   virtual ConnectivityAgentError receiveRawArray(UInt8* pArray, UInt32 &size);

private:
   // Types section

   /**
    * State of handshake
    */
   enum EHandshakeState
   {
      eHandshakeBefore, ///< Handshake began
      eHandshakeA,      ///< Greeting messages were received
      eHandshakeError,  ///< Handshke failed
      eHandshakeDone    ///< Handshake done, addresses were exchanged
   };

private:
   // from CThread
   // Next methods are hidden

   virtual bool start();
   virtual bool stop();
   virtual bool stop(CSignalSemaphore *const pSem);
   virtual void threadFunc();

private:
   // Methods section

   /**
    * Constructor
    * @param connectionInfo is used to establish connection
    */
   explicit CTcpCarrierAdapter(const CTcpConnectionInfo & connectionInfo);

   /**
    * isBroken() check without mutexes
    * @return mIsBroken value
    */
   bool isBrokenUnprotected() const;

   /**
    * Performs actions needed to laucnh server-side listening socket.
    * @param sock file descriptor of socket
    * @param addr socket address information used by ::bind()
    * @retval ERR_OK listen successfuly began, mLsnSocket was set
    * @retval ERR_FAIL some error
    */
   ConnectivityAgentError listen(int sock, addrinfo* addr);

   /**
    * Performs actions needed to laucnh client-side connecting socket.
    * Changes the "broken" state.
    * @param sock file descriptor of socket
    * @param addr socket address information used by ::connect()
    * @retval ERR_OK connection successfull established
    * @retval ERR_FAIL some error
    */
   ConnectivityAgentError connect(int sock, addrinfo* addr);

   /**
    * Changes socket settings
    * @param sock file descriptor of socket
    * @retval ERR_OK settings successfull changed
    * @retval ERR_FAIL some error
    */
   ConnectivityAgentError setSocketOptions(int sock);

   /**
    * Accepts incoming connection. 
    * If accept is successful, listenig socket is closed.
    * Changes the "broken" state.
    */
   void acceptConnection();

   /**
    * Closes all opened sockets - listen and connected ones.
    */
   void closeAllSockets();

   /**
    * Closes connected socket
    */
   void closeClientSocket();

   /**
    * Closes listen socket
    * @param needLock enables mutex usage
    */
   void closeListenSocket(bool needLock);

   /**
    * Performs actual socket closing
    * @param sock file descriptor of socket
    */
   void closeSocket(int sock);

   /**
    * Called in working thread on data receive on connected socket availability.
    * Also, will be
    * called if other side closes connection.
    * @retval true all ok
    * @retval false connection lost
    */
   bool consumeData();

   /**
    * Helper method, waits handshake state change.
    * Uses mHandshakeTimeout
    * @param state desired handshake state
    * @retval ERR_OK handshake in desired state
    * @retval ERR_FAIL timed out before handshake got desired state
    */
   ConnectivityAgentError waitHandshakeState(EHandshakeState state);

   /**
    * Called in working thread to receive handshake data.
    * Changes current handshake state.
    * @retval true all ok
    * @retval false handshake is in error state, connection lost
    */
   bool processHandshake();

   /**
    * Sets current remote address
    * @param pAddress pointer to string with address representation
    */
   void setRemoteAddress(const char* const pAddress);

   /**
    * Sets current local address
    * @param pAddress pointer to string with address representation
    */
   void setLocalAddress(const char* const pAddress);

   /**
    * @retval "TCP/IP"
    */
   virtual const char* getTypeName() const;

private:
   // Members section

   /// Information used to establish connection
   const CTcpConnectionInfo mConnectionInfo;

   /// "Broken" state
   bool mIsBroken;

   /// File descriptor of socket used for data exchange
   int mSocket;

   /// File descriptor of socket used for incomming connections listening
   int mLsnSocket;

   /// Mutex used to block access to sockets
   mutable CRWMutex mSocketMutex;

   /// Data structure used by ::select()
   fd_set mRFdSet;

   /// State of handshake
   EHandshakeState mHandshakeState;

   /// Condvar used to protect handshake state and to wait its change
   CCondVar mHandshakeStateCondVar;

   /// Timeout value used for handshake state change waiting
   time_t mHandshakeTimeout;

   /// String representation of remote IP-address.
   /// Returned by getRemoteAddress() method.
   /// Allocated by strdup() with malloc(), so must be deallocated by free.
   char* mpRemoteAddress;

   /// String representation of local IP-address, known to remote side.
   /// Returned by getLocalAddress() method.
   /// Allocated by strdup() with malloc(), so must be deallocated be free.
   char* mpLocalAddress;

   /// Signals which can be send to working thread using selfpipe
   enum ESignals
   {
      eGameOver  ///< connection lost, thread must stop
   };

   /**
    * Helper class used to pass signals to working thread and to wake ::select()
    * from its blocked waiting.
    */
   class CSelfpipe
   {
   public:
      typedef ESignals tSignal;
   public:
      CSelfpipe();
      ~CSelfpipe();

      ConnectivityAgentError setup();
      void destroy();

      bool isReady() const;

      int getReadingEnd() const;
      ConnectivityAgentError signal(tSignal signal);
      ConnectivityAgentError readSignal(tSignal& signal);

   private:
      bool mReady;
      int mSelfpipe[2];
   private:
      static Logger msLogger;
   };

   CSelfpipe mSelfpipe;
};

}  // namespace HAL
}  // namespace ConnectivityAgent
}  // namespace AXIS


#endif /* CTCPCARRIERADAPTER_HPP_ */
