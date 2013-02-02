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
#include "VersionCompatibility.hpp"

using iviLink::ConnectivityAgent::HAL::CTcpCarrierAdapter;
using namespace iviLink::ConnectivityAgent;

Logger CTcpCarrierAdapter::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CTcpCarrierAdapter"));

// "AXIS" and "SIXA" - handshake part, 2 last bytes - for version
UInt8 handshakeAndVerStrAxis[] = "AXIS  ";
UInt8 handshakeAndVerStrSixa[] = "SIXA  ";

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
   mSocketMutex.unlockRead();
   return res;
}


ConnectivityAgentError CTcpCarrierAdapter::connect()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   mSocketMutex.lockWrite();

   if (!isBrokenUnprotected())
   {
      mSocketMutex.unlockWrite();
      LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::connect() !isBrokenUnprotected");
      return ConnectivityAgentError::NoError();
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

         return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
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

      if (!setSocketOptions(sock).isNoError())
      {
         LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::connect() setSocketOptions failed");
         close(sock);
         continue;
      }

      if (mConnectionInfo.serverSocket)
      {
         if (!this->listen(sock, res).isNoError())
         {
            LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::connect() listen failed");
            close(sock);
            continue;
         }
      }
      else
      {
         if (!this->connect(sock, res).isNoError())
         {
            LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::connect() connect failed");
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

   mSocketMutex.unlockWrite();

   if (found)
   {
      LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::connect() found");
      start();
      return ConnectivityAgentError::NoError();
   }

   LOG4CPLUS_ERROR(logger, "CTcpCarrierAdapter::connect() failed");
   return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
}

ConnectivityAgentError CTcpCarrierAdapter::sendPreparedArray(const UInt8* pArray, const UInt32 size)
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
   mSocketMutex.unlockRead();

   if (-1 == n)
   {
      mSelfpipe.signal(eGameOver);

      return ConnectivityAgentError(ConnectivityAgentError::ERROR_BROKEN_PIPE);
   }
   else
   {
      assert(sent == size);
   }

   return ConnectivityAgentError::NoError();
}


ConnectivityAgentError CTcpCarrierAdapter::receiveRawArray(UInt8* pArray, UInt32 & size)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   ssize_t n = -1;
   int saved_errno = 0;

   ConnectivityAgentError receiveResult = ConnectivityAgentError::NoError();

   mSocketMutex.lockRead();
   {
      do {
         n = ::recv(mSocket, pArray, size, 0);
      } while (n == -1 && errno == EINTR);

      saved_errno = errno;
   }
   mSocketMutex.unlockRead();

   if (n > 0)
   {
      size = n;
   }
   else
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::receiveRawArray() recv error: "
         + convertIntegerToString(saved_errno) + strerror(saved_errno));
      // connection lost
      mSelfpipe.signal(eGameOver);
      receiveResult.setErrorCode(ConnectivityAgentError::ERROR_BROKEN_PIPE);
   }

   return receiveResult;
}


bool CTcpCarrierAdapter::isBrokenUnprotected() const
{
   return mIsBroken;
}

ConnectivityAgentError CTcpCarrierAdapter::listen(int sock, addrinfo* addr)
{
   if (-1 == ::bind(sock, addr->ai_addr, addr->ai_addrlen))
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::listen() bind error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }

   if (-1 == ::listen(sock, 1))
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::listen() listen error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }

   mLsnSocket = sock;

   return ConnectivityAgentError::NoError();
}

ConnectivityAgentError CTcpCarrierAdapter::connect(int sock, addrinfo* addr)
{
   if (-1 == ::connect(sock, addr->ai_addr, addr->ai_addrlen))
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::connect() connect error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }

   const size_t MAXSOCKADDR = 128;
   sockaddr *sa = reinterpret_cast<sockaddr*> (malloc(MAXSOCKADDR));
   socklen_t len = MAXSOCKADDR;
   if (getpeername(sock, sa, &len) < 0)
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::connect(): getpeername error: "
         + convertIntegerToString(errno) + strerror(errno));
      free(sa);

      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }
   else
   {
      const char* pAddr = sockNtoPHost_r(sa, len);
      free(sa);

      setRemoteAddress(pAddr);
      delete [] pAddr;
   }

   mSocket = sock;

   return ConnectivityAgentError::NoError();
}

ConnectivityAgentError CTcpCarrierAdapter::setSocketOptions(int sock)
{
   const int on = 1;
   if (mConnectionInfo.serverSocket && 
      -1 == ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::setSocketOptions() setsockopt SO_REUSEADDR error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }

   if (-1 == fcntl(sock, F_SETFD, FD_CLOEXEC))
   {
      LOG4CPLUS_WARN(logger, "CTcpCarrierAdapter::setSocketOptions() fcntl FD_CLOEXEC error: "
         + convertIntegerToString(errno) + strerror(errno));
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }

   return ConnectivityAgentError::NoError();
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
         LOG4CPLUS_WARN(logger, BaseError::FormErrnoDescr(errno).c_str());
      }
      else
      {
         LOG4CPLUS_INFO(logger, "CIpcSocket::acceptConnection() lsn " 
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
   mSocketMutex.unlockWrite();

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
   mSocketMutex.unlockWrite();
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
      mSocketMutex.unlockWrite();
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
      LOG4CPLUS_ERROR(logger, BaseError::FormErrnoDescr(errno).c_str());
   }
}


void CTcpCarrierAdapter::threadFunc()
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);
   fd_set* const rSet = &mRFdSet;

   bool needExit = false;

   while (true)
   {
      LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::threadFunc() iteration");
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
            FD_SET(mLsnSocket, rSet);
            maxFD = std::max(mLsnSocket, maxFD);
         }

         if (mSocket != -1)
         {
            FD_SET(mSocket, rSet);
            maxFD = std::max(mSocket, maxFD);
         }

         // Must be max+1
         ++maxFD;

         int res = -1;
         do
         {
            LOG4CPLUS_INFO(logger, "before select maxFD = " + convertIntegerToString(maxFD));
            res = ::select(maxFD, rSet, NULL, NULL, NULL);
         } while (res == -1 && errno == EINTR);
         LOG4CPLUS_INFO(logger, "select res = " + convertIntegerToString(res));

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
      mSocketMutex.unlockRead();

      if (needExit)
         break;

      if (pipeSet)
      {
         CSelfpipe::tSignal sig;
         if (!mSelfpipe.readSignal(sig).isNoError())
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


ConnectivityAgentError CTcpCarrierAdapter::makeHandshake()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   ConnectivityAgentError handshakeResult = ConnectivityAgentError::NoError();

   mHandshakeStateCondVar.lock();
   if (mHandshakeState != eHandshakeBefore)
   {
      LOG4CPLUS_ERROR(logger,"wrong handshake state" );
      handshakeResult.setErrorCode(ConnectivityAgentError::ERROR_HANDSHAKE_FAILED);
   }
   mHandshakeStateCondVar.unlock();

   if (!handshakeResult.isNoError())
      return handshakeResult;

   // will return this in case of any handshake fail
   ConnectivityAgentError failedResult(ConnectivityAgentError::ERROR_HANDSHAKE_FAILED);

   const UInt32 size = 50;
   UInt8 pAddr[size];
   memset(pAddr, 0, size);
   strncpy(reinterpret_cast<char*>(pAddr), this->getRemoteAddress(), size);

   if (mConnectionInfo.serverSocket)
   {
      LOG4CPLUS_INFO(logger, "CTcpCarrierAdapter::makeHandshake(): send AXIS ...");

      handshakeAndVerStrAxis[4] = IVILINK_VERSION_MAJOR;
      handshakeAndVerStrAxis[5] = IVILINK_VERSION_MINOR;
      if (!sendPreparedArray(handshakeAndVerStrAxis, sizeof(handshakeAndVerStrAxis)).isNoError())
      {
         LOG4CPLUS_ERROR(logger,"handshake AXIS send failed" );
         return failedResult;
      }

      if (!waitHandshakeState(eHandshakeA).isNoError())
      {
         LOG4CPLUS_ERROR(logger,"handshake not state A");
         return failedResult;
      }

      if (!sendPreparedArray(pAddr, sizeof(pAddr)).isNoError())
      {
         LOG4CPLUS_ERROR(logger,"handshake send addr failed");
         return failedResult;
      }

      if (!waitHandshakeState(eHandshakeDone).isNoError())
      {
         LOG4CPLUS_ERROR(logger,"handshake not state done");
         return failedResult;
      }

      mGenderType = eServerGender;
   }
   else
   {
      if (!waitHandshakeState(eHandshakeA).isNoError())
      {
         LOG4CPLUS_ERROR(logger,"handshake not state A");
         return failedResult;
      }

      handshakeAndVerStrSixa[4] = IVILINK_VERSION_MAJOR;
      handshakeAndVerStrSixa[5] = IVILINK_VERSION_MINOR;
      if (!sendPreparedArray(handshakeAndVerStrSixa, sizeof(handshakeAndVerStrSixa)).isNoError())
      {
         LOG4CPLUS_ERROR(logger,"handshake SIXA send failed" );
         return failedResult;
      }

      if (!waitHandshakeState(eHandshakeDone).isNoError())
      {
         LOG4CPLUS_ERROR(logger,"handshake not state done");
         return failedResult;
      }

      if (!sendPreparedArray(pAddr, sizeof(pAddr)).isNoError())
      {
         LOG4CPLUS_ERROR(logger,"handshake send addr failed");
         return failedResult;
      }

      mGenderType = eClientGender;
   }

   return ConnectivityAgentError::NoError();
}

ConnectivityAgentError CTcpCarrierAdapter::waitHandshakeState(EHandshakeState state)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   ConnectivityAgentError waitResult = ConnectivityAgentError::NoError();

   mHandshakeStateCondVar.lock();
   {
      int rc = 0;
      timespec ts = CCondVar::calcAbsTimeout(mHandshakeTimeout);
      while (mHandshakeState != state && rc == 0)
      {
         rc = mHandshakeStateCondVar.waitTimeout(ts);
      }
      if (rc != 0 || mHandshakeState != state)
         waitResult.setErrorCode(ConnectivityAgentError::ERROR_HANDSHAKE_FAILED);
   }
   mHandshakeStateCondVar.unlock();

   return waitResult;
}

bool CTcpCarrierAdapter::processHandshake()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   const size_t buf_size = 50;
   UInt32 size = 0;
   UInt8 buf[buf_size];
   bool ret = true;

   mHandshakeStateCondVar.lock();
   switch (mHandshakeState)
   {
   case eHandshakeBefore:
      {
         bool receivedOk = false;
         if (mConnectionInfo.serverSocket)
         {
            size = sizeof(handshakeAndVerStrSixa);
            receivedOk = receiveRawArray(buf, size).isNoError() &&
                           (0 == memcmp(buf, handshakeAndVerStrSixa,
                                        sizeof(handshakeAndVerStrSixa) - IVILINK_VERSION_SIZE - 1));
            if (!receivedOk)
            {
                mHandshakeState = eHandshakeError;
                ret = false;
                LOG4CPLUS_ERROR(logger,"handshake SIXA recv error");
            }
         }
         else
         {
             size = sizeof(handshakeAndVerStrAxis);
             receivedOk = receiveRawArray(buf, size).isNoError() &&
                            (0 == memcmp(buf, handshakeAndVerStrAxis,
                                        sizeof(handshakeAndVerStrAxis) - IVILINK_VERSION_SIZE - 1));
             if (!receivedOk)
             {
                 mHandshakeState = eHandshakeError;
                 ret = false;
                 LOG4CPLUS_ERROR(logger,"handshake AXIS recv error");
             }
         }

         if (receivedOk)
         {
            UInt16 otherSideVer = (buf[4] << 8) | buf[5];
            Utils::VersionCompatibility compat;
            int compatResult = compat.checkCompatibility(
                    (IVILINK_VERSION_MAJOR << 8) | IVILINK_VERSION_MINOR, otherSideVer);
            LOG4CPLUS_INFO(logger,"IVILink versions. Our : "
                      + convertIntegerToString(IVILINK_VERSION_MAJOR)
                      + "." + convertIntegerToString(IVILINK_VERSION_MINOR)
                      + " , other side : "
                      + convertIntegerToString(buf[4]) + "."
                      + convertIntegerToString(buf[5]));
            if (compatResult == Utils::IVILINK_COMPATIBLE)
            {
               mHandshakeState = eHandshakeA;
               LOG4CPLUS_INFO(logger,"IVILink versions are compatible. handshake set state A");
            }
            else if (compatResult == Utils::IVILINK_NOT_COMPATIBLE)
            {
               LOG4CPLUS_WARN(logger,"IVILink versions are not compatible");
               mHandshakeState = eHandshakeError;
               ret = false;
            }
            else if (compatResult == Utils::IVILINK_UNKNOWN_VERSION)
            {
               LOG4CPLUS_INFO(logger,
                       "IVILink versions: unknown compatibility. handshake -> state A");
               mHandshakeState = eHandshakeA;
            }
            else
            {
               LOG4CPLUS_ERROR(logger, "incorrect compatibility value!");
               mHandshakeState = eHandshakeError;
               ret = false;
            }
         }
      }
      break;
   case eHandshakeA:
      {
         size = buf_size;
         if (receiveRawArray(buf, size).isNoError())
         {
            setLocalAddress(reinterpret_cast<char const*>(buf));
            mHandshakeState = eHandshakeDone;
            LOG4CPLUS_INFO(logger,"handshake set state DONE");
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
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(logger, (std::string)"Remote address: " + mpRemoteAddress);
   return mpRemoteAddress;
}

const char* CTcpCarrierAdapter::getLocalAddress() const
{
   return mpLocalAddress;
}

void CTcpCarrierAdapter::setRemoteAddress(const char* const pAddress)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
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
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
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

const char* CTcpCarrierAdapter::getTypeName() const
{
   return "TCP/IP";
}

bool CTcpCarrierAdapter::start()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return CThread::start();
}

bool CTcpCarrierAdapter::stop()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return CThread::stop();
}

bool CTcpCarrierAdapter::stop(CSignalSemaphore *const pSem)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return CThread::stop(pSem);
}


CTcpCarrierAdapter::CSelfpipe::CSelfpipe()
   : mReady(false)
{
   mSelfpipe[0] = mSelfpipe[1] = -1;
   mReady = (setup().isNoError());
}

CTcpCarrierAdapter::CSelfpipe::~CSelfpipe()
{
   destroy();
}

ConnectivityAgentError CTcpCarrierAdapter::CSelfpipe::setup()
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);
   int res = ::pipe(mSelfpipe);

   if (-1 == res)
   {
      int saved_errno = errno;
      LOG4CPLUS_ERROR(logger, "pipe error: " + convertIntegerToString(saved_errno)
         + strerror(saved_errno));
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }
   
   if(-1 == fcntl(mSelfpipe[1], F_SETFL, fcntl(mSelfpipe[1], F_GETFL) | O_NONBLOCK))
   {
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }

   if (-1 == fcntl(mSelfpipe[0], F_SETFD, FD_CLOEXEC))
   {
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }
   if (-1 == fcntl(mSelfpipe[1], F_SETFD, FD_CLOEXEC))
   {
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CONNECT_FAILED);
   }

   return ConnectivityAgentError::NoError();
}

void CTcpCarrierAdapter::CSelfpipe::destroy()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

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

ConnectivityAgentError CTcpCarrierAdapter::CSelfpipe::signal(tSignal signal)
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);
   ConnectivityAgentError error = ConnectivityAgentError::NoError();
   ssize_t res = -1;

   do {
      res = write(mSelfpipe[1], &signal, sizeof(signal));
   } while (res == -1 && errno == EINTR);

   if (res == -1)
   {
      int saved_errno = errno;
      LOG4CPLUS_ERROR(logger, "selfpipe write error: "
         + convertIntegerToString(saved_errno) + strerror(saved_errno));
      error.setErrorCode(ConnectivityAgentError::ERROR_OTHER);
   }

   return error;
}

ConnectivityAgentError CTcpCarrierAdapter::CSelfpipe::readSignal(tSignal& signal)
{
   LOG4CPLUS_TRACE(logger, __PRETTY_FUNCTION__);
   ConnectivityAgentError error = ConnectivityAgentError::NoError();
   ssize_t res = -1;

   do {
      res = read(mSelfpipe[0], &signal, sizeof(signal));
   } while (res == -1 && errno == EINTR);

   if (res == -1)
   {
      int saved_errno = errno;
      LOG4CPLUS_ERROR(logger, "selfpipe write error: "
         + convertIntegerToString(saved_errno) + strerror(saved_errno));
      error.setErrorCode(ConnectivityAgentError::ERROR_OTHER);
   }

   return error;
}
