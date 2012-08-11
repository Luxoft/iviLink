/* 
 * 
 * iviLINK SDK, version 1.0.1
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





#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "CTcpCarrierAdapter.hpp"
#include "SocketUtils.hpp"

#include "utils/misc/CError.hpp"

using iviLink::ConnectivityAgent::HAL::CTcpCarrierAdapter;

Logger CTcpCarrierAdapter::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CTcpCarrierAdapter"));

const UInt8 gHstr1[] = "AXIS";
const UInt8 gHstr2[] = "SIXA";


CTcpCarrierAdapter::CTcpCarrierAdapter(const CTcpConnectionInfo & connectionInfo) :
   CThread("CTcpCarrierAdapter", true),
   mConnectionInfo(connectionInfo),
   mIsBroken(true),
   mSocket(-1),
   mLsnSocket(-1),
   mHandshakeState(eHandshakeError),
   mHandshakeTimeout(3000),
   mpRemoteAddress(NULL),
   mpLocalAddress(NULL)
{
   assert(mSelfpipe.isReady());
}

CTcpCarrierAdapter::~CTcpCarrierAdapter()
{
   // There is "possible data race during write" here message from 
   // helgrind 3.7.0 and I do not understand how it is possible. Race is between 
   // reading from threadFunc() and writing from this destructor. And we 
   // specifically messaging threadFunc() using selfpipe and waiting until it
   // stops using CThread::join().
   // It may be just an error in helgrind, but I'm not sure.

   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   
   disconnect();

   CThread::join();

   setRemoteAddress(NULL);
   setLocalAddress(NULL);
}

void CTcpCarrierAdapter::disconnect()
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);
   mSelfpipe.signal(eGameOver);
}

bool CTcpCarrierAdapter::isBroken() const
{
   bool res = false;
   mSocketMutex.lockRead();
   res = isBrokenUnprotected();
   mSocketMutex.unlock();
   return res;
}


ERROR_CODE CTcpCarrierAdapter::connect()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   mSocketMutex.lockWrite();

   if (!isBrokenUnprotected())
   {
      mSocketMutex.unlock();
      LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::connect() !isBrokenUnprotected");
      return ERR_OK;
   }

   addrinfo hints;
   addrinfo* res = NULL;

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   if (mConnectionInfo.serverSocket)
   {
      hints.ai_flags = AI_PASSIVE;
   }
   //        hints.ai_protocol = IPPROTO_TCP; /// !!!

   {
      int n;
      char const* host = mConnectionInfo.host.c_str();
      char const* serv = mConnectionInfo.service.c_str();
      if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
      {
         LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::connect() error for "
            + std::string(host) + ", "
            + std::string(serv) + ": "
            + std::string(gai_strerror(n)));

         return ERR_FAIL;
      }
   }
   addrinfo *const ressave = res;

   bool found = false;
   for (; res; res = res->ai_next)
   {
      int sock = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
      if (sock == -1)
      {
         int saved_errno = errno;
         LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::connect() socket error: "
            + convertIntegerToString(saved_errno) + strerror(saved_errno));

         continue;
      }

      if (ERR_OK != setSocketOptions(sock))
      {
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::connect() setSocketOptions failed");
         close(sock);
         continue;
      }

      if (mConnectionInfo.serverSocket)
      {
         if (ERR_OK != this->listen(sock, res))
         {
            LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::connect() listen failed");
            close(sock);
            continue;
         }
      }
      else
      {
         if (ERR_OK != this->connect(sock, res))
         {
            LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::connect() connect failed");
            close(sock);
            continue;
         }

         mHandshakeState = eHandshakeBefore;
         mIsBroken = false;
      }

      found = true;
      break;
   }

   freeaddrinfo(ressave);

   mSocketMutex.unlock();

   if (found)
   {
      LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::connect() found");
      start();
      return ERR_OK;
   }

   LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::connect() failed");
   return ERR_FAIL;
}

ERROR_CODE CTcpCarrierAdapter::sendPrepearedArray(const UInt8* pArray, UInt32 size)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   ssize_t n = -1;
   size_t sent = 0;

   mSocketMutex.lockRead();
   {
      do {
         n = ::send(mSocket, pArray + sent, size - sent, MSG_NOSIGNAL);
         
         if (n != -1)
         {
            sent += n;
         }
      } while ((n != -1 && sent < size) || 
               (n == -1 && errno == EINTR));
   }
   mSocketMutex.unlock();

   if (-1 == n)
   {
      mSelfpipe.signal(eGameOver);

      return ERR_FAIL;
   }
   else
   {
      assert(sent == size);
   }

   return ERR_OK;
}


ERROR_CODE CTcpCarrierAdapter::receiveRawArray(UInt8* pArray, UInt32 & size)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   ssize_t n = -1;
   int saved_errno = 0;

   ERROR_CODE ret = ERR_OK;

   mSocketMutex.lockRead();
   {
      do {
         n = ::recv(mSocket, pArray, size, 0);
      } while (n == -1 && errno == EINTR);

      saved_errno = errno;
   }
   mSocketMutex.unlock();

   if (n > 0)
   {
      size = n;
   }
   else if (n == 0)
   {
      // connection lost
      mSelfpipe.signal(eGameOver);
      ret = ERR_FAIL;
   }
   else
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::receiveRawArray() recv error: "
         + convertIntegerToString(saved_errno) + strerror(saved_errno));
      mSelfpipe.signal(eGameOver);
      ret = ERR_FAIL;
   }

   return ret;
}



bool CTcpCarrierAdapter::isBrokenUnprotected() const
{
   return mIsBroken;
}

ERROR_CODE CTcpCarrierAdapter::listen(int sock, addrinfo* addr)
{
   if (-1 == ::bind(sock, addr->ai_addr, addr->ai_addrlen))
   {
      //return CIpcError(CIpcError::ERROR_COMMUNICATION, CError::FormErrnoDescr(errno));
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::listen() bind error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ERR_FAIL;
   }

   if (-1 == ::listen(sock, 1))
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::listen() listen error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ERR_FAIL;
   }

   mLsnSocket = sock;

   return ERR_OK;
}

ERROR_CODE CTcpCarrierAdapter::connect(int sock, addrinfo* addr)
{
   if (-1 == ::connect(sock, addr->ai_addr, addr->ai_addrlen))
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::connect() connect error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ERR_FAIL;
   }

   const size_t MAXSOCKADDR = 128;
   sockaddr *sa = reinterpret_cast<sockaddr*> (malloc(MAXSOCKADDR));
   socklen_t len = MAXSOCKADDR;
   if (getpeername(sock, sa, &len) < 0)
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::connect(): getpeername error: "
         + convertIntegerToString(errno) + strerror(errno));
      free(sa);

      return ERR_FAIL;
   }
   else
   {
      const char* pAddr = sockNtoPHost_r(sa, len);
      free(sa);

      setRemoteAddress(pAddr);
      delete [] pAddr;
   }

   mSocket = sock;

   return ERR_OK;
}

ERROR_CODE CTcpCarrierAdapter::setSocketOptions(int sock)
{
   const int on = 1;

   if (mConnectionInfo.serverSocket && 
      -1 == ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::setSocketOptions() setsockopt SO_REUSEADDR error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ERR_FAIL;
   }

   if (-1 == fcntl(sock, F_SETFD, FD_CLOEXEC))
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::setSocketOptions() fcntl FD_CLOEXEC error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ERR_FAIL;
   }

   return ERR_OK;
}

void CTcpCarrierAdapter::acceptConnection()
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);

   const size_t MAXSOCKADDR = 128;
   sockaddr *sa = reinterpret_cast<sockaddr*> (malloc(MAXSOCKADDR));
   socklen_t len = MAXSOCKADDR;

   mSocketMutex.lockWrite();
   {
      int sock = ::accept(mLsnSocket, sa, &len);
      if (sock == -1)
      {
         LOG4CPLUS_WARN(logger, CError::FormErrnoDescr(errno).c_str());
      }
      else
      {
         LOG4CPLUS_DEBUG(logger, "CIpcSocket::acceptConnection() lsn " 
            + convertIntegerToString(mLsnSocket)
            + " new_sock " + convertIntegerToString(sock));

         closeListenSocket(false);

         setSocketOptions(sock);

         mSocket = sock;

         mIsBroken = false;

         mHandshakeState = eHandshakeBefore;

         const char* addr = sockNtoPHost_r(sa, len);
         setRemoteAddress(addr);
         delete [] addr;
      }      
   }
   mSocketMutex.unlock();

   free(sa);
}

void CTcpCarrierAdapter::closeAllSockets()
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);

   mSocketMutex.lockWrite();
   {
      closeListenSocket(false);

      closeClientSocket();

   }
   mSocketMutex.unlock();
}

void CTcpCarrierAdapter::closeClientSocket()
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);

   if (mSocket != -1)
      closeSocket(mSocket);

   mSocket = -1;
   mIsBroken = true;
   mHandshakeState = eHandshakeBefore;
}

void CTcpCarrierAdapter::closeListenSocket(bool needLock)
{
   if (needLock)
      mSocketMutex.lockWrite();

   if (mLsnSocket != -1)
   {
      closeSocket(mLsnSocket);

      mLsnSocket = -1;
   }

   if (needLock)
      mSocketMutex.unlock();
}

void CTcpCarrierAdapter::closeSocket(int sock)
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__ + convertIntegerToString(sock));

   int res = -1;
   do
   {
      res = ::close(sock);
   } while (res == -1 && errno == EINTR);

   if (res == -1)
   {
      LOG4CPLUS_ERROR(logger, CError::FormErrnoDescr(errno).c_str());
   }
}




void CTcpCarrierAdapter::threadFunc()
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);
   fd_set* const rSet = &mRFdSet;

   bool needExit = false;

   while (true)
   {
      LOG4CPLUS_TRACE(logger, "CTcpCarrierAdapter::threadFunc() iteration");
      bool sockSet = false;
      bool lsnSet  = false;
      bool pipeSet = false;

      mSocketMutex.lockRead();
      {
         FD_ZERO(rSet);

         int maxFD = -1;

         FD_SET(mSelfpipe.getReadingEnd(), rSet);
         maxFD = std::max(mSelfpipe.getReadingEnd(), maxFD);

         if (mLsnSocket != -1)
         {
            //LOG4CPLUS_DEBUG(logger, "Setting mLsnSocket = " + convertIntegerToString(mLsnSocket));
            FD_SET(mLsnSocket, rSet);
            maxFD = std::max(mLsnSocket, maxFD);
         }

         if (mSocket != -1)
         {
         //    LOG4CPLUS_DEBUG(logger, "Setting mReadingSocket = " + convertIntegerToString(mReadingSocket.sock)
         //                         + ", dir = " + convertIntegerToString(mReadingSocket.dirId));
            FD_SET(mSocket, rSet);
            maxFD = std::max(mSocket, maxFD);
         }

         // Must be max+1
         ++maxFD;

         int res = -1;
         do
         {
            LOG4CPLUS_TRACE(logger, "before select maxFD = " + convertIntegerToString(maxFD));
            res = ::select(maxFD, rSet, NULL, NULL, NULL);
         } while (res == -1 && errno == EINTR);
         LOG4CPLUS_DEBUG(logger, "select res = " + convertIntegerToString(res));

         if (res == -1)
         {
            /// @todo error handling
            needExit = true;
         }
         else
         {
            pipeSet = FD_ISSET(mSelfpipe.getReadingEnd(), rSet);

            if (mLsnSocket != -1)
            {
               lsnSet = FD_ISSET(mLsnSocket, rSet);
            }

            if (mSocket != -1)
            {
               sockSet = FD_ISSET(mSocket, rSet);
            }
         } // if res < -1
      }
      mSocketMutex.unlock();

      if (needExit)
         break;

      if (pipeSet)
      {
         CSelfpipe::tSignal sig;
         if (ERR_OK != mSelfpipe.readSignal(sig))
         {
            break;
         }

         switch (sig)
         {
         case eGameOver:
            needExit = true;
            break;
         }

         if (needExit)
            break;

         continue;
      }

      if (lsnSet)
      {
         acceptConnection();

         continue;
      }

      if (sockSet && !consumeData())
      {
         break;
      }

   } // while

   closeAllSockets();
}


bool CTcpCarrierAdapter::consumeData()
{
   if (mHandshakeState == eHandshakeDone)
   {
      haveData();
   }
   else
   {
      return processHandshake();
   }

   return true;
}


ERROR_CODE CTcpCarrierAdapter::makeHandshake()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   ERROR_CODE ret = ERR_OK;

   mHandshakeStateCondVar.lock();
   if (mHandshakeState != eHandshakeBefore)
   {
      LOG4CPLUS_ERROR(logger,"wrong handshake state" );
      ret = ERR_FAIL;
   }
   mHandshakeStateCondVar.unlock();

   if (ERR_OK != ret)
      return ret;

   const UInt32 size = 50;
   UInt8 pAddr[size];
   memset(pAddr, 0, size);
   strncpy(reinterpret_cast<char*>(pAddr), this->getRemoteAddress(), size);

   if (mConnectionInfo.serverSocket)
   {
      LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::makeHandshake(): send AXIS ...");

      if (ERR_OK != sendPrepearedArray(gHstr1, sizeof(gHstr1)))
      {
         LOG4CPLUS_ERROR(logger,"handshake AXIS send failed" );
         return ERR_FAIL;
      }

      if (ERR_OK != waitHandshakeState(eHandshakeA))
      {
         LOG4CPLUS_ERROR(logger,"handshake not state A");
         return ERR_FAIL;
      }

      if (ERR_OK != sendPrepearedArray(pAddr, sizeof(pAddr)))
      {
         LOG4CPLUS_ERROR(logger,"handshake send addr failed");
         return ERR_FAIL;
      }

      if (ERR_OK != waitHandshakeState(eHandshakeDone))
      {
         LOG4CPLUS_ERROR(logger,"handshake not state done");
         return ERR_FAIL;
      }

      mGenderType = eServerGender;
   }
   else
   {
      if (ERR_OK != waitHandshakeState(eHandshakeA))
      {
         LOG4CPLUS_ERROR(logger,"handshake not state A");
         return ERR_FAIL;
      }

      if (ERR_OK != this->sendPrepearedArray(gHstr2, sizeof(gHstr2)))
      {
         LOG4CPLUS_ERROR(logger,"handshake SIXA send failed" );
         return ERR_FAIL;
      }

      if (ERR_OK != waitHandshakeState(eHandshakeDone))
      {
         LOG4CPLUS_ERROR(logger,"handshake not state done");
         return ERR_FAIL;
      }

      if (ERR_OK != sendPrepearedArray(pAddr, sizeof(pAddr)))
      {
         LOG4CPLUS_ERROR(logger,"handshake send addr failed");
         return ERR_FAIL;
      }

      mGenderType = eClientGender;
   }

   return ERR_OK;
}

ERROR_CODE CTcpCarrierAdapter::waitHandshakeState(EHandshakeState state)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   ERROR_CODE ret = ERR_OK;

   mHandshakeStateCondVar.lock();
   {
      int rc = 0;
      timespec ts = CCondVar::calcAbsTimeout(mHandshakeTimeout);
      while (mHandshakeState != state && rc == 0)
      {
         rc = mHandshakeStateCondVar.waitTimeout(ts);
      }
      if (rc != 0 || mHandshakeState != state)
         ret = ERR_FAIL;
   }
   mHandshakeStateCondVar.unlock();

   return ret;
}

bool CTcpCarrierAdapter::processHandshake()
{
   const size_t buf_size = 50;
   UInt32 size = 0;
   UInt8 buf[buf_size];
   bool ret = true;

   mHandshakeStateCondVar.lock();
   switch (mHandshakeState)
   {
   case eHandshakeBefore:
      {
         if (mConnectionInfo.serverSocket)
         {
            size = sizeof(gHstr2);
            if (ERR_OK == receiveRawArray(buf, size) &&
               0 == memcmp(buf, gHstr2, sizeof(gHstr2)))
            {
               mHandshakeState = eHandshakeA;
               LOG4CPLUS_TRACE(logger,"handshake set state A");
            }
            else
            {
               mHandshakeState = eHandshakeError;
               ret = false;
               LOG4CPLUS_ERROR(logger,"handshake SIXA recv error");
            }
         }
         else
         {
            size = sizeof(gHstr1);
            if (ERR_OK == receiveRawArray(buf, size) &&
               0 == memcmp(buf, gHstr1, sizeof(gHstr1)))
            {
               mHandshakeState = eHandshakeA;
               LOG4CPLUS_TRACE(logger,"handshake set state A");
            }
            else
            {
               mHandshakeState = eHandshakeError;
               ret = false;
               LOG4CPLUS_ERROR(logger,"handshake AXIS recv error");
            }
         }
      }
      break;
   case eHandshakeA:
      {
         size = buf_size;
         if (ERR_OK == receiveRawArray(buf, size))
         {
            setLocalAddress(reinterpret_cast<char const*>(buf));
            mHandshakeState = eHandshakeDone;
            LOG4CPLUS_TRACE(logger,"handshake set state DONE");
         }
         else
         {
            mHandshakeState = eHandshakeError;
            ret = false;
            LOG4CPLUS_ERROR(logger,"handshake addr recv error");
         }
      }
      break;
   case eHandshakeError:
   case eHandshakeDone:
      {
         mHandshakeState = eHandshakeError;
         ret = false;
         LOG4CPLUS_ERROR(logger,"handshake wrong state for recv");
      }
      break;
   }

   mHandshakeStateCondVar.signal();
   mHandshakeStateCondVar.unlock();

   return ret;
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
         + std::string(mpRemoteAddress ? mpRemoteAddress : "(null)")
         + ", new = " + std::string(pAddress ? pAddress : "(null)"));
   if (mpRemoteAddress)
   {
      free(mpRemoteAddress);
   }
   if (pAddress)
   {
      mpRemoteAddress = strdup(pAddress);
   }
}

void CTcpCarrierAdapter::setLocalAddress(const char* const pAddress)
{
   LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::setLocalAddress old = "
        + std::string(mpRemoteAddress ? mpRemoteAddress : "(null)") 
        + ", new = " + std::string(pAddress ? pAddress : "(null)"));
   if (mpLocalAddress)
   {
      free(mpLocalAddress);
      mpLocalAddress = NULL;
   }
   if (pAddress)
   {
      mpLocalAddress = strdup(pAddress);
   }
}

bool CTcpCarrierAdapter::start()
{
   return CThread::start();
}

bool CTcpCarrierAdapter::stop()
{
   return CThread::stop();
}

bool CTcpCarrierAdapter::stop(CSignalSemaphore *const pSem)
{
   return CThread::stop(pSem);
}


CTcpCarrierAdapter::CSelfpipe::CSelfpipe()
   : mReady(false)
{
   mSelfpipe[0] = mSelfpipe[1] = -1;
   mReady = (ERR_OK == setup());
}

CTcpCarrierAdapter::CSelfpipe::~CSelfpipe()
{
   destroy();
}

ERROR_CODE CTcpCarrierAdapter::CSelfpipe::setup()
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);
   int res = ::pipe(mSelfpipe);
   if (-1 == res)
   {
      int saved_errno = errno;
      LOG4CPLUS_ERROR(logger, "pipe error: " + convertIntegerToString(saved_errno)
         + strerror(saved_errno));
      return ERR_FAIL;
   }
   /// @todo error handling in fcntl
   //fcntl(mSelfpipe[0], F_SETFL, fcntl(mSelfpipe[0], F_GETFL) | O_NONBLOCK);
   fcntl(mSelfpipe[1], F_SETFL, fcntl(mSelfpipe[1], F_GETFL) | O_NONBLOCK);

   fcntl(mSelfpipe[0], F_SETFD, FD_CLOEXEC);
   fcntl(mSelfpipe[1], F_SETFD, FD_CLOEXEC);

   return ERR_OK;
}

void CTcpCarrierAdapter::CSelfpipe::destroy()
{
   LOG4CPLUS_TRACE(logger, "destroySelfpipe()");
   /// @todo error handling
   close(mSelfpipe[0]);
   close(mSelfpipe[1]);

   mSelfpipe[0] = mSelfpipe[1] = -1;
}

bool CTcpCarrierAdapter::CSelfpipe::isReady() const
{
   return mReady;
}

int CTcpCarrierAdapter::CSelfpipe::getReadingEnd() const
{
   return mSelfpipe[0];
}

ERROR_CODE CTcpCarrierAdapter::CSelfpipe::signal(tSignal signal)
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);
   ERROR_CODE err = ERR_OK;
   ssize_t res = -1;

   do {
      res = write(mSelfpipe[1], &signal, sizeof(signal));
   } while (res == -1 && errno == EINTR);

   if (res == -1)
   {
      int saved_errno = errno;
      LOG4CPLUS_ERROR(logger, "selfpipe write error: "
         + convertIntegerToString(saved_errno) + strerror(saved_errno));
      err = ERR_FAIL;
   }

   return err;
}

ERROR_CODE CTcpCarrierAdapter::CSelfpipe::readSignal(tSignal& signal)
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);
   ERROR_CODE err = ERR_OK;
   ssize_t res = -1;

   do {
      res = read(mSelfpipe[0], &signal, sizeof(signal));
   } while (res == -1 && errno == EINTR);

   if (res == -1)
   {
      int saved_errno = errno;
      LOG4CPLUS_ERROR(logger, "selfpipe write error: "
         + convertIntegerToString(saved_errno) + strerror(saved_errno));
      err = ERR_FAIL;
   }

   return err;
}
