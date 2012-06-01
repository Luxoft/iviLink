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
 * @file             CTcpAutoConnectionFinder.cpp
 * @brief            CTcpAutoConnectionFinder class implementation file
 * @author           PIlin
 ***************************************************************************/

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <assert.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/rtnetlink.h>

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "CNetlinkSocket.hpp"
#include "CTcpCarrierAdapter.hpp"

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CTcpAutoConnectionFinder.hpp"

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/misc/include/Types.hpp"
#include "CAdapterFactory.hpp"
#include "utils/misc/include/Logger.hpp"


static const char BRD_REQUEST[] =  "AXIS???";
static const char BRD_RESPONSE[] = "!!!SIXA";

Logger CTcpAutoConnectionFinder::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CTcpAutoConnectionFinder"));

CTcpAutoConnectionFinder::CTcpAutoConnectionFinder(CSignalSemaphore* sem, bool isServer) :
   CConnectionFinder(sem, isServer),
	mpNetlinkSocket(NULL),
	mBroadcastSock(0)
{
   if (!isServer)
   {
	   mpNetlinkSocket = new CNetlinkSocket;
   }

   mBroadcastSock = socket(AF_INET, SOCK_DGRAM, 0);
   if (!mBroadcastSock)
   {
      LOG4CPLUS_ERROR(logger, "CTcpAutoConnectionFinder socket error: " + string(strerror(errno)));
      return;
   }

   int optval = 1;
   setsockopt(mBroadcastSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
   setsockopt(mBroadcastSock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));

   timeval tv;
   tv.tv_sec = 1;
   tv.tv_usec = 0;
   setsockopt(mBroadcastSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

   if (isServer)
   {
	   sockaddr_in servaddr;
	   memset(&servaddr, 0, sizeof(servaddr));
	   servaddr.sin_family = AF_INET;
	   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	   servaddr.sin_port = htons(AXIS_TCP_AUTO_CONNECTION_FINDER_BRD_PORT);

	   if (!bind(mBroadcastSock, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr)))
	   {
	      LOG4CPLUS_ERROR(logger, "CTcpAutoConnectionFinder bind error: " + string(strerror(errno)));
	   }
   }

   start();
}

CTcpAutoConnectionFinder::~CTcpAutoConnectionFinder()
{
   stop();
   if (mBroadcastSock)
   {
	   close(mBroadcastSock);
   }

   delete mpNetlinkSocket;
}

void CTcpAutoConnectionFinder::threadFunc()
{
   if (mIsServer)
   {
      CCarrierAdapter* pAdapter = createServerCarrierAdapter();

      while (!getStopFlag() && pAdapter->isBroken())
      {
         waitForClient();
      }

      setReady(pAdapter);
   }
   else
   {
      while (!getStopFlag() && !checkInterfaces())
      {
         CThread::sleep(2000);
      }
   }
}

void CTcpAutoConnectionFinder::waitForClient()
{
	LOG4CPLUS_TRACE(logger, "CTcpAutoConnectionFinder::waitForClient");

	sockaddr_in cli_addr;
	socklen_t cli_addr_len = sizeof(cli_addr);
	static char buffer[10];

	ssize_t res = recvfrom(mBroadcastSock, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&cli_addr), &cli_addr_len);
	if (-1 == res)
	{
		switch (errno)
		{
		case EAGAIN:
			return;
		default:
			LOG4CPLUS_ERROR(logger, "CTcpAutoConnectionFinder::waitForClient recvfrom error: " + string(strerror(errno)));
			return;
		}
	}
	else if (res != sizeof(BRD_REQUEST) || strncmp(buffer, BRD_REQUEST, sizeof(BRD_REQUEST)))
	{
		LOG4CPLUS_WARN(logger, "CTcpAutoConnectionFinder::waitForClient got wrong request of size " + convertIntegerToString((int)res));
		return;
	}
	else
	{
		// ok
		
		const char* addr = CTcpCarrierAdapter::sockNtopHost(reinterpret_cast<sockaddr*>(&cli_addr), cli_addr_len);
		LOG4CPLUS_INFO(logger, "CCTcpAutoConnectionFinder::waitForClient correct response from " + string(addr));

		if (sizeof(BRD_RESPONSE) != sendto(mBroadcastSock, BRD_RESPONSE, sizeof(BRD_RESPONSE), 0, reinterpret_cast<sockaddr*>(&cli_addr), sizeof(cli_addr)))
		{
			LOG4CPLUS_ERROR(logger, "CTcpAutoConnectionFinder::waitForClient sendto error: " + string(strerror(errno)));
		}
		
	}
}


bool CTcpAutoConnectionFinder::findServer(const in_addr& brd_addr)
{
	LOG4CPLUS_TRACE(logger, "CTcpAutoConnectionFinder::findServer");

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr = brd_addr;
	addr.sin_port = htons(AXIS_TCP_AUTO_CONNECTION_FINDER_BRD_PORT);

	if (sizeof(BRD_REQUEST) != sendto(mBroadcastSock, BRD_REQUEST, sizeof(BRD_REQUEST), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)))
	{
		LOG4CPLUS_ERROR(logger, "CTcpAutoConnectionFinder::findServer sendto error "
				+ convertIntegerToString(errno) + " " + string(strerror(errno)));
		return false;
	}

	sockaddr_in srv_addr;
	socklen_t srv_addr_len = sizeof(srv_addr);
	static char buffer[10];

	ssize_t res = recvfrom(mBroadcastSock, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&srv_addr), &srv_addr_len);
	if (res == -1)
	{
		switch (errno)
		{
		case EAGAIN:
			return false;
		default:
			LOG4CPLUS_ERROR(logger, "CTcpAutoConnectionFinder::findServer recvfrom error: " + string(strerror(errno)));
			return false;
		}
	}
	else if (res != sizeof(BRD_RESPONSE) || strncmp(buffer, BRD_RESPONSE, sizeof(BRD_RESPONSE)))
	{
		LOG4CPLUS_WARN(logger, "CTcpAutoConnectionFinder::findServer got wrong response of size " + convertIntegerToString((int)res));
		return false;
	}
	else
	{
		// ok
		
		mConnectionInfo.host = std::string(CTcpCarrierAdapter::sockNtopHost(reinterpret_cast<sockaddr*>(&srv_addr), srv_addr_len));

		LOG4CPLUS_INFO(logger, "CCTcpAutoConnectionFinder::findServer correct response from " + mConnectionInfo.host);

		return createClientCarrierAdapter();
	}
	
}

void CTcpAutoConnectionFinder::nlmsgParse(nlmsghdr* pHdr)
{
   switch (pHdr->nlmsg_type)
   {
   case RTM_NEWADDR:
   {
      LOG4CPLUS_INFO(logger, "CTcpAutoConnectionFinder::nlmsgParse RTM_NEWADDR message");
      parseIfaceMsg(pHdr);
      break;
   }
   default:
      LOG4CPLUS_WARN(logger, "CTcpAutoConnectionFinder::nlmsgParse unknown message");
      break;
   }
}

bool CTcpAutoConnectionFinder::parseIfaceMsg(const nlmsghdr* h)
{
   LOG4CPLUS_INFO(logger, "finder: iface message");
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
         LOG4CPLUS_INFO(logger, "finder: IFLA_IFNAME " + string(ifName));
         break;
      default:
         break;
      }
   }

   // Pointer to structure with information about network interface
   ifaddrmsg *ifa = (struct ifaddrmsg*) NLMSG_DATA(h);
   
   in_addr ifBrdcast;
   bool wasBrdcast = false;

   rtAttr = IFA_RTA(ifa);
   rtLen = IFA_PAYLOAD(h);
   for (; RTA_OK(rtAttr,rtLen); rtAttr = RTA_NEXT(rtAttr,rtLen))
   {
      const rtattr& crt = *rtAttr;

      switch (crt.rta_type)
      {
      case IFA_BROADCAST:
         LOG4CPLUS_INFO(logger, "finder: IFA_BROADCAST: " + string(inet_ntoa(*(in_addr*)RTA_DATA(&crt))));
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
   return false;
}

bool CTcpAutoConnectionFinder::checkInterfaces()
{
   LOG4CPLUS_TRACE(logger, "finder: checkInterfaces()");

   mBrdAddresses.clear();

   CNetlinkSocket::Request req;

   memset(&req, 0, sizeof(req));

   // Fill in the nlmsg header
   req.nlh.nlmsg_type = RTM_GETADDR;
   req.nlh.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
   req.g.rtgen_family = AF_INET;
   
   if (mpNetlinkSocket->sendRequest(req) < 0)
   {
      return false;
   }

   ssize_t msgLen = mpNetlinkSocket->recvRequest(*this);
   if (msgLen < 0)
   {
      return false;
   }

   for (tBrdAddresses::iterator it = mBrdAddresses.begin(); it != mBrdAddresses.end(); ++it)
   {
      if (findServer(*it))
      {
         return true;
      }
   }

   return false;
}

bool CTcpAutoConnectionFinder::createClientCarrierAdapter()
{
   mConnectionInfo.serverSocket = false;
   mConnectionInfo.service = std::string(AXIS_PORT);

   LOG4CPLUS_INFO(logger, "CTcpAutoConnectionFinder: creating client adapter");

   assert(mpAdapter == NULL);

   CTcpCarrierAdapter* pAdapter = static_cast<CTcpCarrierAdapter*>(CAdapterFactory::CreateTcpAdapter(mConnectionInfo));

   if (pAdapter->isBroken())
   {
      delete pAdapter;
      return false;
   }
   else
   {
      setReady(pAdapter);
      stop();
      return true;
   }
}

CCarrierAdapter* CTcpAutoConnectionFinder::createServerCarrierAdapter()
{
   mConnectionInfo.serverSocket = true;
   mConnectionInfo.service = std::string(AXIS_PORT);
   mConnectionInfo.host = std::string("0.0.0.0");

   LOG4CPLUS_INFO(logger, "CTcpAutoConnectionFinder: creating server adapter");

   assert(mpAdapter == NULL);

   CTcpCarrierAdapter* pAdapter = static_cast<CTcpCarrierAdapter*>(CAdapterFactory::CreateTcpAdapter(mConnectionInfo));
   pAdapter->start();
   return pAdapter;
}
