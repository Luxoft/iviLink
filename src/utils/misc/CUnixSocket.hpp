/* 
 * 
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */











#ifndef CUNIXSOCKET_HPP_
#define CUNIXSOCKET_HPP_

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <sys/socket.h>
#include <sys/un.h>
#include <string>

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/threads/CThread.hpp"
#include "utils/threads/CMutex.hpp"
#include "Types.hpp"
#include "Logger.hpp"

/********************************************************************
 *
 * Forward declarations
 *
 ********************************************************************/
class IUnixSocketSubscriber;

/// CUnixSocket class
/**
 * Class encapsulates the usage of "Unix socket" mechanism.
 */
class CUnixSocket : public CThread
{
private:
   static Logger logger;

public:
   // Methods section

   /**
    * COnstructor
    * @param pPath - path to unix socket
    * @param isServerSocket - flag is true if server awaiting socket is created
    * @param pSubscriber - pointer to IUnixSocketSubscriber interface implementor, who will receive data
    */
   CUnixSocket(const char* pPath, bool isServerSocket, IUnixSocketSubscriber* pSubscriber, bool askForReadiness = false, 
#if defined(ANDROID)
	bool abstractPath = true);
#else
	bool abstractPath = false);
#endif // ANDROID

   /**
    * Destructor
    */
   ~CUnixSocket();

   /**
    * Initializes connection to specified path
    * @return true if connection is successful
    */
   bool connect();

   bool listen();

   /**
    * Send the specified data array
    * @param data - pointer to data array
    * @param data_size - size of data array
    * @return operation error code
    */
   ERROR_CODE send(const UInt8* data, UInt32 data_size);


   /**
    * Closes socket
    * @return
    */
   ERROR_CODE close();


   /**
    * Returns connection state.
    * Must be called after connect() method.
    * @return true if socket is connected
    */
   bool isConnected();

   const std::string& getPath() const { return mPath; }

protected:

   virtual void threadFunc();

private:
   // Methods section

   CUnixSocket(bool isServerSocket, IUnixSocketSubscriber* pSubscriber, int connectedSocket, bool askForReadiness);


   /**
    * Copying constructor.
    * Unimplemented to avoid misusing.
    * @param classRef The reference to the object the class is constructing on.
    */
   CUnixSocket(const CUnixSocket& classRef);

   /**
    * Assigning operator.
    * Unimplemented to avoid misusing.
    * @param classRef The reference to the object the class is assigning from.
    */
   CUnixSocket& operator=(const CUnixSocket& classRef);

   /**
    * Sets timeout for receive operations
    * @param sec - seconds
    * @param usec - nanoseconds
    *
    * @todo this function is duplicate of CTcpCarrierAdapter::setReceiveTimeout. May be we need to
    * make more general method for sockets
    */
   void setReceiveTimeout(__time_t sec, __suseconds_t usec);

   /**
    * Waits for incoming data and receives them into mBuffer.
    * @param[out] received_size - size of received data
    * @return operation error code
    *
    * If receive is interrupted by timeout or signal, function will return ERR_OK and received_size = 0.
    */
   ERROR_CODE receive(UInt32& received_size, bool peek = false);

private:
   // Members section

   /// Pointer to instance of IUnixSocketSubscriber interface that will get received data
   IUnixSocketSubscriber* mpSubscriber;

   /// Awaiting connection socket flag
   bool mIsServerSocket;

   /// Socket will ask subscriber about readiness to receive data
   bool mAskForReadiness;

   /// Socket will use abstract namespace used for unix sockets in linux environment.
   bool mAbstractPath;

   /// Socket address structure
   sockaddr_un mAddr;

   /// Socket address structure used by server socket
   sockaddr_un mRemoteAddr;

   /// Socket file descriptor. All data transmission will be made through this socket.
   int mSock;

   /// Socket file descriptor used by server socket
   int mRemoteSock;

   /// Mutex used to protect receive operations and mBuffer
   CMutex mReceiveMutex;

   /// Buffer array used to receive data
   UInt8 mBuffer[MAX_SIZE];

   /// Flag used to indicate socket destruction
   bool mDestroyed;

   bool mIncomingListen;

   std::string mPath;
};


#endif /* CUNIXSOCKET_HPP_ */
