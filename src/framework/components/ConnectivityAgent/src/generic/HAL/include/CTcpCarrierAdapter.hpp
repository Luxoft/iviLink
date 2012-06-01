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
 * @file             CTcpCarrierAdapter.hpp
 * @brief            CTcpCarrierAdapter class header file
 * @author           VPlachkov
 ***************************************************************************/

#ifndef CTCPCARRIERADAPTER_HPP_
#define CTCPCARRIERADAPTER_HPP_

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <string>
#include <vector>

#include <sys/socket.h>

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/threads/include/CThread.hpp"
#include "utils/threads/include/CMutex.hpp"
#include "utils/threads/include/CRWMutex.hpp"
#include "utils/threads/include/CSignalSemaphore.hpp"
#include "ConnectionInfo.hpp"
#include "CCarrierAdapter.hpp"

struct sockaddr;

/// CTcpCarrierAdapter class
/**
 * Class is used to manage connection by TCP/IP socket.
 * It encapsulates connection establishing and data transfer.
 *
 * Connection is established when newly created object of class if not "broken" (isBroken()).
 *
 * Object is operationg in separate thread to listen for connections and wait for new incoming data.
 *
 * Server side object creates listening socket and waits in separate thread until new connection is
 * established.
 * Client side object tries to connect to specified address in the thread of object creator.
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
   friend class CAdapterFactory;

public:
   // Methods section

   /**
    * Destructor
    */
   virtual ~CTcpCarrierAdapter();
   virtual bool isBroken() const;
   virtual ERROR_CODE makeHandshake();
   virtual const char* getRemoteAddress() const;
   virtual const char* getLocalAddress() const;

   /**
    * Converts socket address structure to string presentation with the help of appropriate function.
    * @param sa - pointer to socket address structure
    * @param salen - length of address structure
    * @return pointer to string representation of address (string allocated statically).
    */
   static const char * sockNtopHost(const sockaddr *sa, socklen_t salen);

protected:
   // Methods section

   virtual ERROR_CODE sendPrepearedArray(const UInt8* pArray, UInt32 size);
   virtual ERROR_CODE receiveRawArray(UInt8* pArray, UInt32 &size);

private:
   // Methods section

   explicit CTcpCarrierAdapter(const CTcpConnectionInfo & connectionInfo);

   /**
    * Makes TCP connection
    * @param host - host IP address, serv - port
    * $return socker descriptor
    */
   int tcpConnect(const char *host, const char *serv);

   /**
    * Creates socket, binds it
    * @param host and serv - like in tcpConnect; addrlenp - size od address
    * $ return socket descriptor
    */
   int tcpListen(const char *hoslt, const char *serv, socklen_t& addrlenp);

   /**
    * Copy from mBuffer to pArray
    * @return size of written data
    */
   UInt32 copyFromBuffer(UInt8* pArray, UInt32 size, UInt32 beginPos = 0);

   virtual void threadFunc();

   /**
    * Sets timeout for receive operations
    * @param sec - seconds
    * @param usec - nanoseconds
    */
   void setReceiveTimeout(__time_t sec, __suseconds_t usec);

   /**
    * Sets remote ip address of connection. Copies provided address
    * to mpRemoteAddress.
    * @param pAddress pointer to c-string wit the remote address
    */
   void setRemoteAddress(const char* const pAddress);

   /**
    * Sets local ip address of connection. Copies provided address
    * to mpLocalAddress.
    * @param pAddress pointer to c-string with the local address
    */
   void setLocalAddress(const char* const pAddress);

private:
   // Members section

   /// Information used to establish connection
   CTcpConnectionInfo mConnectionInfo;

   /// Mutex used to lock writing
   CMutex mWriteMutex;

   /// Mutex used to lock reading
   CMutex mReadMutex;

   /// Mutex used to lock socket changing
   mutable CRWMutex mSocketMutex;

   /// Socket identifier
   Int32 mSocket;

   /// Internal data buffer
   UInt8 mBuffer[MAX_SIZE];

   /// Identifier of socket used to listen for new connection
   Int32 mListenfd;

   ///
   socklen_t mServerAddrLen;

   /// Used to wait until handshake is complete
   CSignalSemaphore mHandshakeSemaphore;

   /// String representation of remote IP-address.
   /// Returned by getRemoteAddress() method.
   /// Allocated by strdup() with malloc(), so must be deallocated by free.
   char* mpRemoteAddress;

   /// String representation of local IP-address, known to remote side.
   /// Returned by getLocalAddress() method.
   /// Allocated by strdup() with malloc(), so must be deallocated be free.
   char* mpLocalAddress;
};

#endif /* CTCPCARRIERADAPTER_HPP_ */
