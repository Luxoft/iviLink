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
 * @file             CTcpConnectionFinder.hpp
 * @brief            CTcpConnectionFinder class header file
 * @author           PIlin
 ***************************************************************************/

#ifndef CTCPCONNECTIONFINDER_HPP_
#define CTCPCONNECTIONFINDER_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CConnectionFinder.hpp"
#include "ConnectionInfo.hpp"

/********************************************************************
 *
 * Forward declarations
 *
 ********************************************************************/
class CNetlinkSocket;
class CCarrierAdapter;
struct nlmsghdr;

/// CTcpConnectionFinder class
/**
 * On client-side, this ConnectionFinder detects default gateway in routing table and tries to establish
 * connection with CTcpCarrierAdapter to it. Detection is made with the help of CNetlinkSocket class,
 * so current class Linux-only.
 * On server side, this class creates CTcpCarrierAdapter and waits for incoming connections. When
 * new incoming connection received, CTcpConnectionFinder signals about it.
 */
class CTcpConnectionFinder: public CConnectionFinder
{
public:
   // Methods section

   /**
    * Constructor
    * @param pSem - semaphore used to signal about found connection
    * @param isServer - flag is true when system is server-side.
    */
   CTcpConnectionFinder(CSignalSemaphore* sem, bool isServer);

   /**
    * Destructor
    */
   virtual ~CTcpConnectionFinder();

protected:
   // Methods section

   /**
    * Method that will work in separate thread.
    */
   virtual void threadFunc();

private:

   /**
    * Parses netlink message to detect possible connection
    * @param buf - data array with netlink message
    * @param length - length of buf array
    */
   void parse(const char* buf, ssize_t length);

   /**
    * Parses message about interface changes.
    * @param h - pointer to header of message
    */
   void parseIfaceMsg(const nlmsghdr* h);

   /**
    * Parses message about routing
    * @param h - pointer to header of message
    */
   void parseRouteMsg(const nlmsghdr* h);

   /**
    * Method makes special query to linux kernel to find default gateway
    */
   void findGateway();

   /**
    * Helper method to create client-side CTcpCarrierAdapter
    */
   void createClientCarrierAdapter();

   /**
    * Helper method to create server-side CTcpCarrierAdapter
    */
   CCarrierAdapter* createServerCarrierAdapter();

private:

   /// Pointer to object of class CNetlinkSocket. It used to interact with Linux kernel.
   CNetlinkSocket* mpNetlinkSocket;

   /// CTcpConnectionInfo structure used to hold connection data.
   CTcpConnectionInfo mConnectionInfo;
};

#endif /* CTCPCONNECTIONFINDER_HPP_ */
