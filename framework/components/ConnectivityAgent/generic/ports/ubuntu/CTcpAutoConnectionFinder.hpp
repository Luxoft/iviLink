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


#ifndef CTCPAUTOCONNECTIONFINDER_HPP_
#define CTCPAUTOCONNECTIONFINDER_HPP_

#include <vector>
#include <arpa/inet.h>

#include "CConnectionFinder.hpp"
#include "ConnectionInfo.hpp"
#include "CNetlinkSocket.hpp"
#include "CCarrierAdapter.hpp"

#include "TcpFinderCommon.hpp"
 
struct nlmsghdr;


namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace HAL
      {
         /// CTcpAutoConnectionFinder class
         /**
          * UDP broadcasting is used to find other machines with iviLINK.
          * Connection finder at each turn performs:
          * 1) generates random number SL
          * 2) gets the lists of local ip addresses and available broadcast
          *    addresses
          * 3) sends the broadcast message with number SL to all available
          *    broadcast addresses
          * 4) waits for broadcast message.
          *    If there is no message in given timeframe, goto 1.
          * 5) if correct message is received, read from it the number SR and
          *    sender address.
          * 6a) if SL < SR, create server tcp carrier adapter
          * 6b) if SL > SR, create client tcp carrier adapter
          * 6c) if SL = SR, goto 1
          * 7) if connection is established in given timeframe, end connection
          *    establishing procedure, otherwise goto 1.
          */
         class CTcpAutoConnectionFinder   : public CConnectionFinder
                                          , public CNetlinkSocket::INlmsgParser
         {
         private:
            static Logger msLogger;

         public:
            // Methods section

            /**
             * Constructor
             * @param handler object which will process found connections
             * @param gender gender of connection
             */
            CTcpAutoConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender);

            /**
             * Destructor
             */
            virtual ~CTcpAutoConnectionFinder();

         protected:
            // Methods section
            // from CConnectionFinder

            virtual void performSearch();
         private:
            // Methods section

            /**
             * Method waits for broadcast message. Using this message, program
             * decides to be server or client
             *
             * @param[out] client_addr address used to create client tcp carrier
             *    adapter. Value is changed only in case when function returns
             *    eCLIENT
             *
             * @retval eSERVER this side will be server side
             * @retval eCLIENT this side will be client side, client_addr was
             *    changed
             * @retval eERROR message was not receive or was incorrect
             */
            eMessageResult waitForMessage(std::string& client_addr);

            /**
             * Sends broadcast request to find new connection
             * @param brd_addr broadcast address to send request
             */
            void sendBroadcastMessage(const in_addr& brd_addr);

            /**
             * Parses message about interface changes.
             * @param h - pointer to header of message
             */
            void parseIfaceMsg(const nlmsghdr* h);

            /**
             * Method makes special query to linux kernel to find existing
             * interfaces and uses sendBroadcastMessage() to send requests to
             * all found interfaces.
             */
            bool checkInterfaces();

            /**
             * Helper method to create client-side CTcpCarrierAdapter
             */
            bool createClientCarrierAdapter(std::string const& addr);

            /**
             * Helper method to create server-side CTcpCarrierAdapter
             */
            CCarrierAdapter* createServerCarrierAdapter();

            /**
             * Uses populated mLocalAddresses to check if given address is local
             * @param addr address to be checked
             * @retval true address is local
             * @retval false otherwise
             */
            bool isLocalAddress(sockaddr const& addr);

         private:
            // from CNetlinkSocket::INlmsgParser

            virtual void nlmsgParse(nlmsghdr* pHdr);

         private:
            // Members section

            /// Pointer to object of class CNetlinkSocket.
            /// It used to interact with Linux kernel.
            CNetlinkSocket* mpNetlinkSocket;

            /// Broadcast socket identifier
            int mBroadcastSock;

            /// CTcpConnectionInfo structure used to hold connection data.
            CTcpConnectionInfo mConnectionInfo;

            typedef std::vector<in_addr> tAddresses;

            /// List of broadcast addresses filled by nlmsgParse
            tAddresses mBrdAddresses;

            /// List of local machine addresses filled by nlmsgParse and used
            /// to filter out self messages
            tAddresses mLocalAddresses;

            /// Random number SL used in current connection procedure turn.
            UInt8  mTurnRandomNumber;

            /// This message buffer is used for all sent message. Text is initialized once
            /// and only random number is changed each turn.
            tBrdMessage mBrdMsg;
         };

      }
   }
}

#endif /* CTCPCONNECTIONFINDER_HPP_ */
