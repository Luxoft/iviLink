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


#ifndef CNETLINKSOCKET_HPP_
#define CNETLINKSOCKET_HPP_

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <sys/socket.h>
#include <linux/rtnetlink.h>

#include "Logger.hpp"

namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace HAL
      {
         /// CNetlinkSocket class
         /**
          * This class is used by CTcpAutoCarrierAdapter to interact with Linux kernel
          * through netlink socket.
          * This code is Linux-only.
          */
         class CNetlinkSocket
         {
         private:
            static Logger logger;

         public:

            /**
             * Structure used to send netlink message
             */
            struct Request
            {
               /// Netlink message header
               struct nlmsghdr nlh;
               /// Message family
               struct rtgenmsg g;
            };

            /**
             * Interface of netlink messages parser
             */
            struct INlmsgParser {
               /**
                * Called by recvRequest on each netlink message
                * @param pHdr pointer to netllink message header
                */
               virtual void nlmsgParse(nlmsghdr* pHdr) = 0;
            };

         public:
            // Methods section

            /**
             * Default constructor
             */
            CNetlinkSocket(__u32 messageGroup = /*RTMGRP_LINK | */RTMGRP_IPV4_IFADDR/* | RTMGRP_IPV4_ROUTE*/);

            /**
             * Destructor
             */
            ~CNetlinkSocket();

            /**
             * Used to receive answer to send request. All data hold in internal data buffer.
             * @param seqNum - sequence number of request. This number must equal request seqNum + 1.
             * @return received message length
             */
            bool recvRequest(INlmsgParser& parser);

            /**
             * Used to send request.
             * @param req - request structure. Fields nlh.nlmsg_type, nlh.nlmsg_flags
             * and g.rtgen_family must be set by method caller, other fields are set internally.
             * @return number of characters sent or -1 on error.
             */
            ssize_t sendRequest(Request& req);

         private:

            /// Netlink socket number
            int fd;

            /// Netlink address structure
            sockaddr_nl mAddr;

            /// Buffer used to receive message by recvmsg. mIov is set as one of the
            /// the fields in struct msghdr.
            iovec mIov;

            /// PID associated with netlink socket.
            __u32 mNetlinkPid;

            /// Message sequence number.
            __u32 mSeqNumber;

         private:
            // Methods section

            /**
             * Used to initialize netlink socket
             * @param messageGroups - list of groups to subscript
             * @return true on success
             */
            bool createSocket(__u32 messageGroups);

            /**
             * Internal receive function. Used to receive full netlink message.
             * Manages the size of buffer mIov.
             * @return received message length or -1 in case of error
             */
            ssize_t recv();

            /**
             * Sets timeout for receive operations
             * @param sec - seconds
             * @param usec - nanoseconds
             */
            void setReceiveTimeout(__time_t sec, __suseconds_t usec);

            /**
             * Expands the size of mIov buffer.
             * @param size new buffer size.
             * @return true on success
             */
            bool expandIovBuf(size_t size);

         };
      }
   }
}

#endif /* CNETLINKSOCKET_HPP_ */
