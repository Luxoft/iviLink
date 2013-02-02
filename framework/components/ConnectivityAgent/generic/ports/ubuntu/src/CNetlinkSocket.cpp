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


/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <unistd.h>

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include"Logger.hpp"

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CNetlinkSocket.hpp"

using namespace iviLink::ConnectivityAgent::HAL;
using namespace std;

Logger CNetlinkSocket::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CNetlinkSocket"));

CNetlinkSocket::CNetlinkSocket(__u32 messageGroup) :
   fd(-1),
   mNetlinkPid(0),
   mSeqNumber(0)
{
   bool good = createSocket(messageGroup);

   mIov.iov_len = 512;
   mIov.iov_base = new char[mIov.iov_len];

   if (good)
   {
      LOG4CPLUS_INFO(logger, "netlink: socket ready");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "netlink: socket error");
   }
}

CNetlinkSocket::~CNetlinkSocket()
{
   if (fd != -1)
   {
      close(fd);
   }
   if (mIov.iov_base)
   {
      delete [] reinterpret_cast<char*>(mIov.iov_base);
   }
}

bool CNetlinkSocket::createSocket(__u32 messageGroups)
{
   fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

   if (fd < 0)
   {
      LOG4CPLUS_ERROR(logger, "Error creating netlink socket: "
            + convertIntegerToString(errno) + " " + string(strerror(errno)));
      return false;
   }

   memset(&mAddr, 0, sizeof(mAddr));

   mAddr.nl_family = AF_NETLINK; // protocol family
   mAddr.nl_groups = messageGroups; // subscription groups
   mAddr.nl_pid = 0; // autobind

   if (bind(fd, (struct sockaddr*) &mAddr, sizeof(mAddr)) < 0)
   {
      LOG4CPLUS_ERROR(logger, "Error bind to netlink socket: "
            + convertIntegerToString(errno) + " " + string(strerror(errno)));
      close(fd);
      fd = -1;
      return false;
   }

   socklen_t slen = sizeof(mAddr);
   if (getsockname(fd, (struct sockaddr*)&mAddr, &slen))
   {
      LOG4CPLUS_ERROR(logger, "Error retrieve netlink addr: "
              + convertIntegerToString(errno) + " " + string(strerror(errno)));
      close(fd);
      fd = -1;
      return false;
   }

   mNetlinkPid = mAddr.nl_pid;
   LOG4CPLUS_INFO(logger, "CNetlinkSocket::createSocket our pid is "
         + convertIntegerToString(mNetlinkPid));

   setReceiveTimeout(3, 0);

   return true;
}

bool CNetlinkSocket::recvRequest(INlmsgParser& parser)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   ssize_t len;

   while (true)
   {
      if ((len = this->recv()) < 0)
      {
         return false;
      }

      for (nlmsghdr* h = reinterpret_cast<nlmsghdr*>(mIov.iov_base);
            NLMSG_OK(h, static_cast<size_t>(len));
            h = NLMSG_NEXT(h, len))
      {
         LOG4CPLUS_INFO(logger, "CNetlinkSocket::recvRequest msg "
               + convertIntegerToString((intptr_t)h) + ": len = "
               + convertIntegerToString(h->nlmsg_len) + ", type = "
               + convertIntegerToString((int)h->nlmsg_type) + ", seq = "
               + convertIntegerToString(h->nlmsg_seq) + ", flags = "
               + convertIntegerToString((int)h->nlmsg_flags));

         switch (h->nlmsg_type)
         {
         case NLMSG_ERROR:
         {
            nlmsgerr* err = reinterpret_cast<nlmsgerr*>(NLMSG_DATA(h));
            if (err->error == 0)
            {
               LOG4CPLUS_ERROR(logger, "CNetlinkSocket::recvRequest NLMSG_ERROR ack");
            }
            else
            {
               int e = -(err->error);
               LOG4CPLUS_ERROR(logger, "CNetlinkSocket::recvRequest NLMSG_ERROR "
                    + convertIntegerToString(e) + " "  + string(strerror(e)));
            }
            break;
         }
         case NLMSG_DONE:
         {
            LOG4CPLUS_INFO(logger, "CNetlinkSocket::recvRequest NLMSG_DONE");
            return true;
         }
         default:
         {
            parser.nlmsgParse(h);
            break;
         }
         }
      }
   }
}

ssize_t CNetlinkSocket::sendRequest(Request& req)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   if (-1 == fd)
   {
      LOG4CPLUS_ERROR(logger, "CNetlinkSocket::send error no fd");
      return -1;
   }

   sockaddr_nl addr;
   addr.nl_family = AF_NETLINK;
   addr.nl_pad = 0;
   addr.nl_groups = 0;
   addr.nl_pid = 0; /* address to kernel */

   req.nlh.nlmsg_len = sizeof(req);
   req.nlh.nlmsg_pid = 0;
   req.nlh.nlmsg_seq = ++mSeqNumber;

   ssize_t res = sendto(fd, (void*)&req, sizeof(req), 0,
         (struct sockaddr*)&addr, sizeof(addr));
   if (res < 0)
   {
      LOG4CPLUS_ERROR(logger, "CNetlinkSocket::send error: "
              + convertIntegerToString(errno) + " " + string(strerror(errno)));
   }

   LOG4CPLUS_INFO(logger, "CNetlinkSocket::send done res = "
         + convertIntegerToString((int)res));
   return res;
}

ssize_t CNetlinkSocket::recv()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   msghdr msg;
   sockaddr_nl addr;

   ssize_t rc;

   if (fd == -1)
   {
      LOG4CPLUS_ERROR(logger, "CNetlinkSocket::recv error no fd");
      return -1;
   }

   while (1)
   {
      msg.msg_control = NULL;
      msg.msg_controllen = 0;
      msg.msg_name = &addr;
      msg.msg_namelen = sizeof(addr);
      msg.msg_iov = &mIov;
      msg.msg_iovlen = 1;
      msg.msg_flags = 0;

      while ((rc = recvmsg(fd, &msg, MSG_PEEK | MSG_TRUNC)) == -1 && errno == EINTR)
      {
      }

      /* make buffer big enough */
      if (rc != -1 && (msg.msg_flags & MSG_TRUNC))
      {
         /* Very new Linux kernels return the actual size needed, older ones always return truncated size */
         if ((size_t)rc == mIov.iov_len)
         {
            rc += 128;
         }
         if (expandIovBuf(rc))
         {
            continue;
         }
      }

      /* read it for real */
      msg.msg_flags = 0;
      while ((rc = recvmsg(fd, &msg, 0)) == -1 && errno == EINTR)
      {
      }
      LOG4CPLUS_INFO(logger, "CNetlinkSocket::recv after recvmsg rc = " + convertIntegerToString((int)rc));

      /* Make sure this is from the kernel */
      if (rc == -1 || addr.nl_pid == 0)
      {
         break;
      }
   }

   if (msg.msg_flags & MSG_TRUNC)
   {
      rc = -1;
      LOG4CPLUS_ERROR(logger, "CNetlinkSocket::recv error TRUNCATED");
   }

   return rc;
}

void CNetlinkSocket::setReceiveTimeout(__time_t sec, __suseconds_t usec)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   timeval tv;
   tv.tv_sec = sec;
   tv.tv_usec = usec;

   setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

bool CNetlinkSocket::expandIovBuf(size_t size)
{
   LOG4CPLUS_INFO(logger, "CNetlinkSocket::expandIovBuf size = "
         + convertIntegerToString((unsigned int)size));
   if (size <= mIov.iov_len)
      return true;

   char* n = new char[size];
   if (!n)
   {
      LOG4CPLUS_ERROR(logger, "CNetlinkSocket::expandIovBuf memory error");
      return false;
   }

   if (mIov.iov_base)
   {
      char* old = reinterpret_cast<char*>(mIov.iov_base);
      std::copy(old, old + mIov.iov_len, n);
      delete old;
   }

   mIov.iov_base = n;
   mIov.iov_len = size;

   return true;
}
