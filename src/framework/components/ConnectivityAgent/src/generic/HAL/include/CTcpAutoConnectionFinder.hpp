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
 * @file             CTcpAutoConnectionFinder.hpp
 * @brief            CTcpAutoConnectionFinder class header file
 * @author           PIlin
 ***************************************************************************/

#ifndef CTCPAUTOCONNECTIONFINDER_HPP_
#define CTCPAUTOCONNECTIONFINDER_HPP_

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <vector>
#include <arpa/inet.h>

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CConnectionFinder.hpp"
#include "ConnectionInfo.hpp"
#include "CNetlinkSocket.hpp"

/********************************************************************
 *
 * Forward declarations
 *
 ********************************************************************/
class CCarrierAdapter;
struct nlmsghdr;

/// CTcpConnectionFinder class
/**
 * On client-side, this ConnectionFinder sends the broadcast request and 
 * listens for the answer from running server side ConnectionFinder.
 * On server-side, this ConnectionFinder listens for the requests and answers
 * to them with its own address.
 */
class CTcpAutoConnectionFinder   : public CConnectionFinder
                                 , public CNetlinkSocket::INlmsgParser
{
private:
   static Logger logger;

public:
   // Methods section

   /**
    * Constructor
    * @param pSem - semaphore used to signal about found connection
    * @param isServer - flag is true when system is server-side.
    */
   CTcpAutoConnectionFinder(CSignalSemaphore* sem, bool isServer);

   /**
    * Destructor
    */
   virtual ~CTcpAutoConnectionFinder();

protected:
   // Methods section

   /**
    * Method that will work in separate thread.
    */
   virtual void threadFunc();

private:

   /**
    * Listens for broadcast requests.
    */
   void waitForClient();

   /**
    * Sends broadcast request to find new connection
    * @param brd_addr broadcast address to send request
    * @return true if connection is found
    */
   bool findServer(const in_addr& brd_addr);

   /**
    * Parses message about interface changes.
    * @param h - pointer to header of message
    */
   bool parseIfaceMsg(const nlmsghdr* h);

   /**
    * Method makes special query to linux kernel to find existing interfaces
    */
   bool checkInterfaces();

   /**
    * Helper method to create client-side CTcpCarrierAdapter
    */
   bool createClientCarrierAdapter();

   /**
    * Helper method to create server-side CTcpCarrierAdapter
    */
   CCarrierAdapter* createServerCarrierAdapter();

   virtual void nlmsgParse(nlmsghdr* pHdr);

private:

   /// Pointer to object of class CNetlinkSocket. It used to interact with Linux kernel.
   CNetlinkSocket* mpNetlinkSocket;

   /// Broadcast socket identifier
   int mBroadcastSock;

   /// CTcpConnectionInfo structure used to hold connection data.
   CTcpConnectionInfo mConnectionInfo;

   typedef std::vector<in_addr> tBrdAddresses;

   /// List of broadcast addresses filled by nlmsgParse
   tBrdAddresses mBrdAddresses;
};

#endif /* CTCPCONNECTIONFINDER_HPP_ */
