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
 * File              CTcpCarrierAdapter.cpp
 * Description
 * @Author           VPlachkov
 ***************************************************************************/

/*
 * Includes for using tcp sockets
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <assert.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
 
/*
 * fcntl.h is used for nonblock sockets
 * */
#include <fcntl.h>

#include <cstdio>

/*
 * Class header include
 */
#include "CTcpCarrierAdapter.hpp"

#include "../include/Frame.hpp"

#include"utils/misc/include/Logger.hpp"

#define  MAXLINE     4096  /* max text line length */
#define  MAXSOCKADDR  128  /* max socket address structure size */

Logger CTcpCarrierAdapter::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CTcpCarrierAdapter"));

CTcpCarrierAdapter::CTcpCarrierAdapter(const CTcpConnectionInfo & connectionInfo) :
   CThread("TcpCarrierAdapterThread"),
   mConnectionInfo(connectionInfo),
   mSocket(0),
   mHandshakeSemaphore(0),
   mpRemoteAddress(NULL),
   mpLocalAddress(NULL)
{
   LOG4CPLUS_TRACE(logger, "CTcpCarrierAdapter::CTcpCarrierAdapter()");

   if (!mConnectionInfo.serverSocket)
   {
      mSocket = tcpConnect(connectionInfo.host.c_str(), connectionInfo.service.c_str());

      LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter: tcpConnect returned " + convertIntegerToString(mSocket));

      sockaddr *sa = reinterpret_cast<sockaddr*> (malloc(MAXSOCKADDR));
      socklen_t len = MAXSOCKADDR;
      if (getpeername(mSocket, sa, &len) < 0)
      {
         LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::CTcpCarrierAdapter(): getpeername error: " + string(strerror(errno)));
         mSocket = -1;
      }
      else
      {
         const char* pAddr = sockNtopHost(sa, len);
         setRemoteAddress(pAddr);
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::CTcpCarrierAdapter(): connected to " + string(pAddr));
         start();
      }
   }
   else
   {
      mConnectionInfo.serverSocket = true;
      mListenfd = tcpListen(connectionInfo.host.c_str(), connectionInfo.service.c_str(),
            mServerAddrLen);
      if (!mListenfd)
         LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter: tcpListen error: " + string(strerror(errno)));
   }
}

CTcpCarrierAdapter::~CTcpCarrierAdapter()
{
   stop();
   mSocketMutex.lockWrite();
   if (mSocket > 0)
   {
      close(mSocket);
   }
   if (mpRemoteAddress)
   {
      free(mpRemoteAddress);
   }
   if(mListenfd)
   {
      close(mListenfd);
   }
   mSocketMutex.unlock();
}

ERROR_CODE CTcpCarrierAdapter::sendPrepearedArray(const UInt8* pArray, UInt32 size)
{
   LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::sendPrepearedArray(pArray = "
             + convertIntegerToString((intptr_t)pArray) + ", size = " + convertIntegerToString(size) + ")");

   mWriteMutex.lock();
   ssize_t n = send(mSocket, pArray, size, 0);
   LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::sendPrepearedArray n = "
           + convertIntegerToString((int)n) + ", errno = "
           + convertIntegerToString(errno) + " " + string(strerror(errno)));

   if (n < 0)
   {
      LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::sendPrepearedArray() - connection lost: "
           + convertIntegerToString(errno) + " " + string(strerror(errno)));

      mSocketMutex.lockWrite();
      mSocket = -1; // TODO : do it more beautiful in future
      mSocketMutex.unlock();

      mWriteMutex.unlock();

      return ERR_CONNECTION_LOST;
   }
   else
   {
      LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::sendPrepearedArray() - array is sent (number of bytes: "
               + convertIntegerToString((int)n) + ")");
      mWriteMutex.unlock();
      return ERR_OK;
   }
}

ERROR_CODE CTcpCarrierAdapter::receiveRawArray(UInt8* pArray, UInt32 & size)
{
   mReadMutex.lock();

   assert(!isBroken());

   assert(sizeof(mBuffer) >= size);
   mSocketMutex.lockRead();
   ssize_t n = recv(mSocket, mBuffer, size, MSG_WAITALL/*0*/);
   int saved_errno = errno;
   mSocketMutex.unlock();

   /// @todo: proper errors handling.
   LOG4CPLUS_INFO(logger, "todo: proper errors handling");

   if (n > 0)
   {
      LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::receiveRawArray: "
          + convertIntegerToString(size) + " bytes received");
      assert(n <= (ssize_t)size);
      //size = n;

      if ((UInt32)n != size)
      {
         LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::receiveRawArray ERR_FAIL");
         size = 0;
         mReadMutex.unlock();
         return ERR_FAIL;
      }

      std::copy(mBuffer, mBuffer + size, pArray);
   }

   mReadMutex.unlock();

   if (n > 0)
   {
      size = n;
      return ERR_OK;
   }
   else if (0 == n)
   {
      LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::receiveRawArray ERR_CONNECTION_LOST errno = " + convertIntegerToString(saved_errno) + strerror(saved_errno));
      return ERR_CONNECTION_LOST;
   }
   else
   {
      size = 0;
      switch (saved_errno)
      {
      case EINTR:
         LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::receiveRawArray interrupted");
         // There is no need to break. Next is EWOULDBLOCK
      case EWOULDBLOCK:
         return ERR_AGAIN;

      default:
         LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::receiveRawArray ERR_CONNECTION_LOST errno = " + convertIntegerToString(saved_errno) + strerror(saved_errno));
         perror("CTcpCarrierAdapter::receiveRawArray");
         return ERR_CONNECTION_LOST;
      }
   }
}

bool CTcpCarrierAdapter::isBroken() const
{
   mSocketMutex.lockRead();
   bool good = mSocket > 0;
   mSocketMutex.unlock();
   if (good)
   {
      return false;
   }
   else
   {
      LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::isBroken() = true");
      return true;
   }
}

int CTcpCarrierAdapter::tcpConnect(const char *host, const char *serv)
{
   int sockfd, n;
   addrinfo hints, *res, *ressave;

   bzero(&hints, sizeof(addrinfo));
   hints.ai_family = AF_INET;//AF_UNSPEC;r
   hints.ai_socktype = SOCK_STREAM;
   //   hints.ai_protocol = IPPROTO_TCP; /// !!!

   if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
   {
      LOG4CPLUS_ERROR(logger, "tcpConnect error for "
         + string(host) + ", "
         + string(serv) + ": "
         + string(gai_strerror(n)));

      return 0;
   }

   ressave = res;

   do
   {
      sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
      if (sockfd < 0)
      {
         continue; /* ignore this one */
      }
      if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
      {
         break; /* success */
      }

      close(sockfd); /* ignore this one */
   } while ((res = res->ai_next) != NULL);

   freeaddrinfo(ressave);

   return (sockfd);
}

int CTcpCarrierAdapter::tcpListen(const char *host, const char *serv, socklen_t &addrlenp)
{
   int listenfd, n;
   const int on = 1;
   struct addrinfo hints, *res, *ressave;

   bzero(&hints, sizeof(struct addrinfo));
   hints.ai_flags = AI_PASSIVE;
   hints.ai_family = AF_INET;//AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   //        hints.ai_protocol = IPPROTO_TCP; /// !!!

   if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
   {
	   LOG4CPLUS_ERROR(logger, "tcp_listen error for "
               + string(host) + ", "
               + string(serv) + ": "
               + string(gai_strerror(n)));
   }
   LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::tcpListen: n = "
         + convertIntegerToString(n) + ", " + (n == 0 ? "OK" : "Error"));
   ressave = res;

   do
   {
      listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
      if (listenfd < 0)
      {
         LOG4CPLUS_ERROR(logger, "next addr errno = " + convertIntegerToString(errno) + " " + string(strerror(errno)));
         continue; /* error, try next one */
      }

      setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
      if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
      {
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::tcpListen: bind() success");
         break; /* success */
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "bind err, next addr errno = " + convertIntegerToString(errno) + " " + string(strerror(errno)));
      }

      close(listenfd); /* bind error, close and try next one */
   } while ((res = res->ai_next) != NULL);

   if (res == NULL) /* errno from final socket() or bind() */
   {
      LOG4CPLUS_ERROR(logger, "tcp_listen error for " + string(host) + ", " + string(serv));
   }
   else
   {
      listen(listenfd, 1024);

      addrlenp = res->ai_addrlen; /* return size of protocol address */

      freeaddrinfo(ressave);
   }

   return listenfd;
}

void CTcpCarrierAdapter::threadFunc()
{
   LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::threadFunc("
         + mConnectionInfo.serverSocket ? "server)" : "client)");

   if (mConnectionInfo.serverSocket)
   {
      struct sockaddr * cliaddr;
      cliaddr = static_cast<sockaddr*> (malloc(mServerAddrLen));
      socklen_t len = mServerAddrLen;
      LOG4CPLUS_INFO(logger, "*CTcpCarrierAdapter* : accepting....");
      int sockfd = accept(mListenfd, cliaddr, &len);
      close(mListenfd);
      mListenfd = 0;
      LOG4CPLUS_INFO(logger, "*CTcpCarrierAdapter* : accept ( ) = "
             + convertIntegerToString(sockfd) + " cliaddr.sa_family = "
             + convertIntegerToString(cliaddr ? cliaddr->sa_family : -1));
      setRemoteAddress(sockNtopHost(cliaddr, len));
      mSocketMutex.lockWrite();
      mSocket = sockfd;
      mSocketMutex.unlock();

      free(cliaddr);
   }

   setReceiveTimeout(3, 0);

   // Waiting until handshake is done
   LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::threadFunc(): waiting until handshake");
   mHandshakeSemaphore.wait();
   LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::threadFunc(): handshake complete");

   Frame frame;
   ERROR_CODE err = ERR_OK;

   while (!getStopFlag())
   {
      err = receiveFrame(frame, err == ERR_AGAIN);
      if (ERR_CONNECTION_LOST == err)
      {
         mSocketMutex.lockWrite();
         if (mSocket > 0)
         {
            close(mSocket);
         }
         mSocket = 0;
         mSocketMutex.unlock();
      }

      if (isBroken())
      {
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::threadFunc() broken. Stopping");
         setStopFlag();
      }
   }
}

ERROR_CODE CTcpCarrierAdapter::makeHandshake()
{
   LOG4CPLUS_TRACE(logger, "CTcpCarrierAdapter::makeHandshake()");
   const UInt8 hstr1[] = "AXIS";
   const UInt8 hstr2[] = "SIXA";
   UInt8 rec_buf[10] = "";
   UInt32 rec_size = 0;

   ERROR_CODE oper_res;
   ERROR_CODE ret = ERR_FAIL;
   int try_count = 0;
   const int max_try_count = 10;

   if (!isBroken())
   {
      if (mConnectionInfo.serverSocket)
      {
         // send "AXIS"
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::makeHandshake(): send AXIS ...");
         if (sendPrepearedArray(hstr1, sizeof(hstr1)) != ERR_OK)
         {
            return ERR_FAIL;
         }

         rec_size = sizeof(hstr2);

         // recv "SIXA"
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::makeHandshake(): recv SIXA ...");
         for (; try_count < max_try_count; ++try_count)
         {
            oper_res = receiveRawArray(rec_buf, rec_size);
            if (oper_res == ERR_CONNECTION_LOST)
            {
               return ERR_FAIL;
            }
            else if (oper_res == ERR_OK)
            {
               break;
            }
         }
         if (try_count == max_try_count)
         {
            return ERR_FAIL;
         }

         // check answer
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::makeHandshake(): check answer ...");

         if (rec_size != sizeof(hstr2))
         {
            return ERR_FAIL;
         }

         for (const UInt8 *p_et = hstr2, *p = rec_buf; *p_et != 0; ++p_et, ++p)
         {
            if (*p != *p_et)
            {
               return ERR_FAIL;
            }
         }
      }
      else
      {
         // recv "AXIS"
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::makeHandshake(): recv AXIS ...");
         rec_size = sizeof(hstr1);
         for (; try_count < max_try_count; ++try_count)
         {
            oper_res = receiveRawArray(rec_buf, rec_size);
            if (oper_res == ERR_CONNECTION_LOST)
            {
               return ERR_FAIL;
            }
            else if (oper_res == ERR_OK)
            {
               break;
            }
         }
         if (try_count == max_try_count)
         {
            return ERR_FAIL;
         }

         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::makeHandshake(): check input ...");
         // check "AXIS"
         if (rec_size != sizeof(hstr1))
         {
            return ERR_FAIL;
         }

         for (const UInt8 *p_et = hstr1, *p = rec_buf; *p_et != 0; ++p_et, ++p)
         {
            if (*p != *p_et)
            {
               return ERR_FAIL;
            }
         }
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::makeHandshake(): send SIXA");
         // send "SIXA"
         if (sendPrepearedArray(hstr2, sizeof(hstr2)) != ERR_OK)
         {
            return ERR_FAIL;
         }
      }

      // send known other side ip
      {
         const UInt32 size = 50;
         char pAddr[size];
         memset(pAddr, 0, size);
         strcpy(pAddr, this->getRemoteAddress());

         if (ERR_OK != sendPrepearedArray(reinterpret_cast<const UInt8*>(pAddr), size))
         {
            return ERR_FAIL;
         }

         // recv our ip known to other side
         for (try_count = 0; try_count < max_try_count; ++try_count)
         {
            LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::makeHandshake try to receive our addr");
            rec_size = size;
            oper_res = receiveRawArray(reinterpret_cast<UInt8*>(pAddr), rec_size);
            if (oper_res == ERR_CONNECTION_LOST)
            {
               return ERR_FAIL;
            }
            else if (oper_res == ERR_OK)
            {
               break;
            }
         }
         if (try_count == max_try_count)
         {
            return ERR_FAIL;
         }

         if (rec_size != size)
         {
            LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::makeHandshake our ip wrong recv size");
            return ERR_FAIL;
         }
         if (strnlen(pAddr, size) == size)
         {
            LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::makeHandshake our ip wrong string size");
            return ERR_FAIL;
         }

         this->setLocalAddress(pAddr);
      }

      mHandshakeSemaphore.signal();

      return ERR_OK;
   }
   else
   {
      return ERR_FAIL;
   }
}

const char * CTcpCarrierAdapter::sockNtopHost(const struct sockaddr *sa, socklen_t salen)
{
   static char str[128]; /* Unix domain is largest */

   switch (sa->sa_family)
   {
   case AF_INET:
   {
      struct sockaddr_in *sin = (struct sockaddr_in *) sa;

      if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
      {
         return (NULL);
      }
      return (str);
   }

#ifdef	IPV6
      case AF_INET6:
      {
         struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;

         if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
         {
            return(NULL);
         }
         return(str);
      }
#endif

#ifdef	AF_UNIX
   case AF_UNIX:
   {
      sockaddr_un *unp = (sockaddr_un *) sa;

      /* OK to have no pathname bound to the socket: happens on
       every connect() unless client calls bind() first. */
      if (unp->sun_path[0] == 0)
      {
         strcpy(str, "(no pathname bound)");
      }
      else
      {
         snprintf(str, sizeof(str), "%s", unp->sun_path);
      }
      return (str);
   }
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
      case AF_LINK:
      {
         struct sockaddr_dl *sdl = (struct sockaddr_dl *) sa;

         if (sdl->sdl_nlen > 0)
         {
            snprintf(str, sizeof(str), "%*s", sdl->sdl_nlen, &sdl->sdl_data[0]);
         }
         else
         {
            snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
         }
         return(str);
      }
#endif
   default:
      snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
      return (str);
   }
   return (NULL);
}

void CTcpCarrierAdapter::setReceiveTimeout(__time_t sec, __suseconds_t usec)
{
   timeval tv;
   tv.tv_sec = sec;
   tv.tv_usec = usec;

   setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

const char* CTcpCarrierAdapter::getRemoteAddress() const
{
   return mpRemoteAddress;
}

const char* CTcpCarrierAdapter::getLocalAddress() const
{
   LOG4CPLUS_TRACE(logger, "CTcpCarrierAdapter::getLocalAddress");
   return mpLocalAddress;
}

void CTcpCarrierAdapter::setRemoteAddress(const char* const pAddress)
{
   LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::setRemoteAddress old = "
         + string(mpRemoteAddress ? mpRemoteAddress : "(null)") + ", new = " + string(pAddress));
   if (mpRemoteAddress)
   {
      free(mpRemoteAddress);
   }
   char* pAddr = strdup(pAddress);
   mpRemoteAddress = pAddr;
}

void CTcpCarrierAdapter::setLocalAddress(const char* const pAddress)
{
   LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::setLocalAddress old = "
        + string(mpRemoteAddress ? mpRemoteAddress : "(null)") + ", new = " + string(pAddress));
   if (mpLocalAddress)
   {
      free(mpLocalAddress);
      mpLocalAddress = NULL;
   }
   mpLocalAddress = strdup(pAddress);
}
