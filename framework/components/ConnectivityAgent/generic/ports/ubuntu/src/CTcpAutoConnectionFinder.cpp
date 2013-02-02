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


#include <cassert>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/rtnetlink.h>

#ifndef ANDROID
#else
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#endif //ANDROID 

#include "CNetlinkSocket.hpp"
#include "CTcpCarrierAdapter.hpp"
#include "CTcpAutoConnectionFinder.hpp"
#include "Types.hpp"
#include "Logger.hpp"
#include "SocketUtils.hpp"

using namespace iviLink::ConnectivityAgent::HAL;
Logger CTcpAutoConnectionFinder::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CTcpAutoConnectionFinder"));

CTcpAutoConnectionFinder::CTcpAutoConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender) :
   CConnectionFinder(handler, gender),
   mpNetlinkSocket(NULL),
   mBroadcastSock(0)
{
   memcpy(mBrdMsg.handshake, BRD_MESSAGE_TEXT, sizeof(mBrdMsg.handshake));
   mBrdMsg.random = 0;

   mpNetlinkSocket = new CNetlinkSocket;

#ifndef ANDROID
   mBroadcastSock = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
#else
   mBroadcastSock = socket(AF_INET, SOCK_DGRAM, 0);
#endif //ANDROID
   if (!mBroadcastSock)
   {
      LOG4CPLUS_ERROR(msLogger, "CTcpAutoConnectionFinder socket error: " + string(strerror(errno)));
      return;
   }

   int optval = 1;
   setsockopt(mBroadcastSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
   setsockopt(mBroadcastSock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));

   timeval tv;
   tv.tv_sec = 1;
   tv.tv_usec = 0;
   setsockopt(mBroadcastSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

   sockaddr_in servaddr;
   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port = htons(AXIS_TCP_AUTO_CONNECTION_FINDER_BRD_PORT);
   if (bind(mBroadcastSock, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr)))
   {
      LOG4CPLUS_ERROR(msLogger, "CTcpAutoConnectionFinder bind error: " + std::string(strerror(errno)));
   }

   srandom(time(NULL));
}

CTcpAutoConnectionFinder::~CTcpAutoConnectionFinder()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   if (mBroadcastSock)
   {
      close(mBroadcastSock);
      mBroadcastSock = 0;
   }

   delete mpNetlinkSocket;
}

void CTcpAutoConnectionFinder::performSearch()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   const int try_count = 10;

   std::string cli_addr;

   mTurnRandomNumber = random() % std::numeric_limits<UInt8>::max();

   LOG4CPLUS_INFO(msLogger, "CTcpAutoConnectionFinder::performSearch() random number = " + std::string(convertIntegerToString(mTurnRandomNumber)));

   mBrdMsg.random = mTurnRandomNumber;

   if (!checkInterfaces())
   {
      return;
   }

   switch (waitForMessage(cli_addr))
   {
   case eSERVER:
      {
         CCarrierAdapter* pSrvAdapter = createServerCarrierAdapter();

         bool connected = false;
         for (int i = 0; i < try_count && pSrvAdapter; ++i)
         {
            if (!pSrvAdapter->isBroken())
            {
               setReady(pSrvAdapter);
               pSrvAdapter = NULL;
               connected = true;
               break;
            }
            else
            {
               this->sleep(FINDER_SLEEP_BETWEEN_RETRIES_MS);
            }
         }

         if (!connected)
         {
            LOG4CPLUS_INFO(msLogger, "CTcpAutoConnectionFinder::performSearch() connection failed, removing server");
            delete pSrvAdapter;
         }
      }
      break;
   case eCLIENT:
      {
         for (int i = 0; i < try_count; ++i)
         {
            if (createClientCarrierAdapter(cli_addr))
            {
               break;
            }
            else
            {
               this->sleep(FINDER_SLEEP_BETWEEN_RETRIES_MS);
            }
         }
      }
      break;
   case eERROR:
      break;
   } // switch

}

eMessageResult CTcpAutoConnectionFinder::waitForMessage(std::string& client_addr)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   sockaddr_in cli_addr;
   sockaddr* const p_cli_addr_base = reinterpret_cast<sockaddr*>(&cli_addr);
   socklen_t cli_addr_len = sizeof(cli_addr);

   /*static */char buffer[sizeof(tBrdMessage)];
   /*static */tBrdMessage const* const in_msg = reinterpret_cast<tBrdMessage const* const>(buffer);

   bool was_local;
   do
   {
      was_local = false;

      ssize_t res = recvfrom(mBroadcastSock, buffer, sizeof(buffer), 0, p_cli_addr_base, &cli_addr_len);
      if (-1 == res)
      {
         switch (errno)
         {
         case EAGAIN:
            return eERROR;
         default:
            LOG4CPLUS_WARN(msLogger, "CTcpAutoConnectionFinder::waitForMessage recvfrom error: " + string(strerror(errno)));
            return eERROR;
         }
      }
      else if (res != sizeof(tBrdMessage) || strncmp(in_msg->handshake, BRD_MESSAGE_TEXT, sizeof(BRD_MESSAGE_TEXT)))
      {
         LOG4CPLUS_WARN(msLogger, "CTcpAutoConnectionFinder::waitForMessage got wrong msg of size " + convertIntegerToString((int)res));
         return eERROR;
      }
      else
      {
         // ok

         const char* addr = sockNtoPHost_r(p_cli_addr_base, cli_addr_len);

         if (!addr)
         {
            LOG4CPLUS_WARN(msLogger, "CCTcpAutoConnectionFinder::waitForMessage unable to get response address");
            return eERROR;
         }

         LOG4CPLUS_INFO(msLogger, "CCTcpAutoConnectionFinder::waitForMessage correct response from " + std::string(addr)
            + "with random number = " + convertIntegerToString(in_msg->random)
            + " (our = " + convertIntegerToString(mTurnRandomNumber) + ")");

         eMessageResult ret = eERROR;

         if (isLocalAddress(*p_cli_addr_base) && mTurnRandomNumber == in_msg->random)
         {
            LOG4CPLUS_INFO(msLogger, "CTcpAutoConnectionFinder::waitForMessage message was local and random number are equal");
            was_local = true;
         }
         else if (mTurnRandomNumber < in_msg->random)
         {
            LOG4CPLUS_INFO(msLogger, "CTcpAutoConnectionFinder::waitForMessage will be server");
            ret = eSERVER;
         }
         else if (mTurnRandomNumber > in_msg->random)
         {
            LOG4CPLUS_INFO(msLogger, "CTcpAutoConnectionFinder::waitForMessage will be client");
            client_addr = std::string(addr);
            ret = eCLIENT;
         }
         else
         {
            LOG4CPLUS_INFO(msLogger, "CTcpAutoConnectionFinder::waitForMessage random numbers are equal");
            ret = eERROR;
         }

         delete [] addr;

         if (!was_local)
         {
            return ret;
         }
      }
   }
   while (was_local);

   return eERROR;
}

void CTcpAutoConnectionFinder::sendBroadcastMessage(const in_addr& brd_addr)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr = brd_addr;
   addr.sin_port = htons(AXIS_TCP_AUTO_CONNECTION_FINDER_BRD_PORT);

   if (sizeof(mBrdMsg) != sendto(mBroadcastSock, &mBrdMsg, sizeof(mBrdMsg), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)))
   {
      LOG4CPLUS_WARN(msLogger, "CTcpAutoConnectionFinder::sendBroadcastMessage sendto error "
            + convertIntegerToString(errno) + " " + string(strerror(errno)));
   }
}

void CTcpAutoConnectionFinder::nlmsgParse(nlmsghdr* pHdr)
{
   switch (pHdr->nlmsg_type)
   {
   case RTM_NEWADDR:
   {
      LOG4CPLUS_INFO(msLogger, "CTcpAutoConnectionFinder::nlmsgParse RTM_NEWADDR message");
      parseIfaceMsg(pHdr);
      break;
   }
   default:
      LOG4CPLUS_WARN(msLogger, "CTcpAutoConnectionFinder::nlmsgParse unknown message");
      break;
   }
}

void CTcpAutoConnectionFinder::parseIfaceMsg(const nlmsghdr* h)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   rtattr* rtAttr = 0;
   int rtLen = 0;

   // Pointer to structure with information about network connection
   ifinfomsg *ifi = static_cast<ifinfomsg*> (NLMSG_DATA(h));

   // Connection name
   char *ifName = NULL;

   rtAttr = IFLA_RTA(ifi);
   rtLen = IFLA_PAYLOAD(h);
   for (; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr,rtLen))
   {
      const rtattr& crt = *rtAttr;
      switch (crt.rta_type)
      {
      case IFLA_IFNAME:
         ifName = (char*) RTA_DATA(&crt);
         LOG4CPLUS_INFO(msLogger, "finder: IFLA_IFNAME " + string(ifName));
         break;
      default:
         break;
      }
   }

   // Pointer to structure with information about network interface
   ifaddrmsg *ifa = (struct ifaddrmsg*) NLMSG_DATA(h);
   
   in_addr ifBrdcast;
   in_addr ifAddress;
   bool wasBrdcast = false;
   bool wasAddress = false;

   rtAttr = IFA_RTA(ifa);
   rtLen = IFA_PAYLOAD(h);
   for (; RTA_OK(rtAttr,rtLen); rtAttr = RTA_NEXT(rtAttr,rtLen))
   {
      const rtattr& crt = *rtAttr;

      switch (crt.rta_type)
      {
      case IFA_LOCAL:
         LOG4CPLUS_INFO(msLogger, "finder: IFA_LOCAL: " + string(inet_ntoa(*(in_addr*)RTA_DATA(&crt))));
         ifAddress = *reinterpret_cast<in_addr*>(RTA_DATA(&crt));
         wasAddress = true;
         break;
      case IFA_BROADCAST:
         LOG4CPLUS_INFO(msLogger, "finder: IFA_BROADCAST: " + string(inet_ntoa(*(in_addr*)RTA_DATA(&crt))));
         ifBrdcast = *reinterpret_cast<in_addr*>(RTA_DATA(&crt));
         wasBrdcast = true;
         break;
      default:
         break;
      }
   }

   if (wasBrdcast)
   {
      mBrdAddresses.push_back(ifBrdcast);
   }
   if (wasAddress)
   {
      mLocalAddresses.push_back(ifAddress);
   }
}

bool CTcpAutoConnectionFinder::checkInterfaces()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   mBrdAddresses.clear();
   mLocalAddresses.clear();

   CNetlinkSocket::Request req;
   memset(&req, 0, sizeof(req));

   // Fill in the nlmsg header
   req.nlh.nlmsg_type = RTM_GETADDR;                  // We need interface addresses
   req.nlh.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;  // The message is a request for dump.
   req.g.rtgen_family = AF_INET;                      // We need IPv4
   
   if (mpNetlinkSocket->sendRequest(req) < 0)
   {
      return false;
   }

   ssize_t msgLen = mpNetlinkSocket->recvRequest(*this);
   if (msgLen < 0)
   {
      return false;
   }

   // Here mBrdAddresses and mLocalAddresses have been already filled.

   for (tAddresses::iterator it = mBrdAddresses.begin(); it != mBrdAddresses.end(); ++it)
   {
      sendBroadcastMessage(*it);
   }

   return true;
}

bool CTcpAutoConnectionFinder::createClientCarrierAdapter(std::string const& addr)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, (std::string)__PRETTY_FUNCTION__ + "addr: " + addr);
   mConnectionInfo.serverSocket = false;
   mConnectionInfo.host = addr;
   mConnectionInfo.service = std::string(AXIS_PORT);

   LOG4CPLUS_INFO(msLogger, "CTcpAutoConnectionFinder: creating client adapter");

   CTcpCarrierAdapter* pAdapter = new CTcpCarrierAdapter(mConnectionInfo);

   if (!pAdapter->connect().isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, "CTcpAutoConnectionFinder: client adapter failed");
      delete pAdapter;
      return false;
   }
   else
   {
      setReady(pAdapter);
      return true;
   }
}

CCarrierAdapter* CTcpAutoConnectionFinder::createServerCarrierAdapter()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mConnectionInfo.serverSocket = true;
   mConnectionInfo.service = std::string(AXIS_PORT);
   mConnectionInfo.host = std::string("0.0.0.0");

   CTcpCarrierAdapter* pAdapter = new CTcpCarrierAdapter(mConnectionInfo);
   if (!pAdapter->connect().isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, "CTcpAutoConnectionFinder: server adapter failed");
      delete pAdapter;
      pAdapter = NULL;
   }
   return pAdapter;
}

bool CTcpAutoConnectionFinder::isLocalAddress(sockaddr const& addr)
{
   if (addr.sa_family == AF_INET)
   {
      for (tAddresses::const_iterator iter = mLocalAddresses.begin(); iter != mLocalAddresses.end(); ++iter)
      {
         in_addr const& local = *iter;
         if (reinterpret_cast<sockaddr_in const* const>(&addr)->sin_addr.s_addr == local.s_addr)
            return true;
      }
   }

   return false;
}
