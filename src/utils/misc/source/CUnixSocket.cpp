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





/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "utils/misc/include/IUnixSocketSubscriber.hpp"

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "utils/misc/include/CUnixSocket.hpp"

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

Logger CUnixSocket::logger = Logger::getInstance(LOG4CPLUS_TEXT("utils.misc.CUnixSocket"));

CUnixSocket::CUnixSocket(const char* pPath, bool isServerSocket, IUnixSocketSubscriber* pSubscriber, bool askForReadiness/* = false*/, bool abstractPath/* = true*/) :
   CThread(pPath),
   mpSubscriber(pSubscriber),
   mIsServerSocket(isServerSocket),
   mAskForReadiness(askForReadiness),
   mAbstractPath(abstractPath),
   mAddr(),
   mRemoteAddr(),
   mSock(-1),
   mRemoteSock(-1),
   mDestroyed(false),
   mIncomingListen(false),
   mPath(pPath)
{
   assert(pPath);
   assert(strlen(pPath) + 1 < sizeof(mAddr.sun_path));
   assert(pSubscriber);

   LOG4CPLUS_INFO(logger, "CUnixSocket this = " + convertIntegerToString((intptr_t)this)
                        + ", path = " + pPath + " (abs = "
                        + convertIntegerToString((intptr_t)abstractPath) + "), isServ = "
                        + convertIntegerToString((intptr_t)isServerSocket) + ", subscr = "
                        + convertIntegerToString((intptr_t)pSubscriber));

   mSock = socket(AF_UNIX, SOCK_STREAM, 0);
   if (mSock == -1)
   {
      LOG4CPLUS_ERROR(logger, "CUnixSocket::CUnixSocket socket error: socket");
      //perror("socket");
   }

   mAddr.sun_family = AF_UNIX;
   if (mAbstractPath)
   {
      strcpy(mAddr.sun_path+1, pPath);
   }
   else
   {
      strcpy(mAddr.sun_path, pPath);
   }
}


CUnixSocket::CUnixSocket(bool isServerSocket, IUnixSocketSubscriber* pSubscriber, int connectedSocket, bool askForReadiness) :
      CThread("CUnixSocket"),
      mpSubscriber(pSubscriber),
      mIsServerSocket(isServerSocket),
      mAskForReadiness(askForReadiness),
      mSock(connectedSocket),
      mRemoteSock(-1),
      mDestroyed(false),
      mIncomingListen(false)
{
   setReceiveTimeout(1, 0);
   start();
}


CUnixSocket::~CUnixSocket()
{
   stop();
   mDestroyed = true;
   close();


}


bool CUnixSocket::connect()
{
   //socklen_t addrLen = strlen(mAddr.sun_path) + sizeof(mAddr.sun_family);
   socklen_t addrLen = sizeof(mAddr);

   if (mIsServerSocket)
   {
      unlink(mAddr.sun_path);

      if (bind(mSock, (sockaddr *) &mAddr, addrLen) == -1)
      {
         LOG4CPLUS_ERROR(logger, "CUnixSocket::connect() "
                       + string(CThread::getName()) + "bind error");
         //perror("CUnixSocket::connect() bind");
         return false;
      }
      else
      {
         if (::listen(mSock, 1) == -1)
         {
            LOG4CPLUS_ERROR(logger, "CUnixSocket::connect() "
                          + string(CThread::getName()) + " listen error");
            //perror("CUnixSocket::connect() listen");
            return false;
         }
         else
         {
            if (mIncomingListen)
            {
               start();
               return true;
            }
            else
            {
               addrLen = (socklen_t) sizeof(mRemoteAddr);
               mRemoteSock = accept(mSock, (sockaddr*) &mRemoteAddr, &addrLen);
               if (mRemoteSock == -1)
               {
                  LOG4CPLUS_ERROR(logger, "CUnixSocket::connect() accept error");
                  //perror("CUnixSocket::connect() accept");
               }
            }

         }
      }

      ::close(mSock);
      mSock = mRemoteSock;
      mRemoteSock = -1;
   }
   else
   {
      if (::connect(mSock, (sockaddr*)&mAddr, addrLen) == -1)
      {
         mSock = -1;
         LOG4CPLUS_ERROR(logger, "CUnixSocket::connect(): "
                       + string(CThread::getName()) + " connect error");
         //perror("CUnixSocket::connect() connect");
         return false;
      }
   }

   setReceiveTimeout(1, 0);

   start();
   return true;
}

bool CUnixSocket::listen()
{
   LOG4CPLUS_ERROR(logger, "CUnixSocket::listen() " +
                   string(CThread::getName()));
   mIncomingListen = true;
   return connect();
}

ERROR_CODE CUnixSocket::close()
{
   mReceiveMutex.lock();


   if (mSock != -1)
   {
      ::close(mSock);
   }

   if (mRemoteSock != -1)
   {
      ::close(mRemoteSock);
   }

   mReceiveMutex.unlock();

   return ERR_OK;
}
   
void CUnixSocket::setReceiveTimeout(__time_t sec, __suseconds_t usec)
{
   timeval tv;
   tv.tv_sec = sec;
   tv.tv_usec = usec;

   setsockopt(mSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

bool CUnixSocket::isConnected()
{
   return mSock != -1;
}

ERROR_CODE CUnixSocket::send(const UInt8* pData, UInt32 data_size)
{
   LOG4CPLUS_INFO(logger, "CUnixSocket::send data = " + convertIntegerToString(*pData)
                        + ", data_size = " + convertIntegerToString(data_size));
   LOG4CPLUS_INFO(logger, "+++++++++++++++++++++++++++++++++SEND DATA++++++++++++++++++++++++++++++++");
   //_hex_print(pData,data_size );
   LOG4CPLUS_INFO(logger, "+++++++++++++++++++++++++++++++++SEND DATA END++++++++++++++++++++++++++++++++");
   ssize_t n = ::send(mSock, pData, data_size, MSG_NOSIGNAL);

   if (n < 0)
   {
      LOG4CPLUS_ERROR(logger, "CUnixSocket::sendPrepearedArray() - "
                    + string(CThread::getName())
                    + " connection lost: " + strerror(errno));

      //mSocketMutex.lockWrite();
      mSock = -1; // TODO : do it more beautiful in future
      //mSocketMutex.unlock();

      //mWriteMutex.unlock();

      assert(mpSubscriber);
      mpSubscriber->onConnectionLost(this);

      return ERR_CONNECTION_LOST;
   }
   else
   {
      LOG4CPLUS_INFO(logger, "CUnixSocket::sendPrepearedArray() - "
         "array is sent (number of bytes: " + convertIntegerToString(n)
         + ") error " + strerror(errno));
      //mWriteMutex.unlock();
      return ERR_OK;
   }
}



ERROR_CODE CUnixSocket::receive(UInt32& received_size, bool peek/* = false*/)
{
   received_size = 0;
   mReceiveMutex.lock();

   int n = recv(mSock, mBuffer, sizeof(mBuffer), peek ? MSG_PEEK : 0);

   mReceiveMutex.unlock();

   if (n > 0)
   {
      received_size = n;
      return ERR_OK;
   }
   else if (n == 0)
   {
      LOG4CPLUS_ERROR(logger, "CUnixSocket::receive() : "
             + string(CThread::getName()) + " => ERR_CONNECTION_LOST");
      return ERR_CONNECTION_LOST;
   }
   else
   {
      switch (errno)
      {
      case EINTR:
         LOG4CPLUS_ERROR(logger, "CUnixSocket::receive(): "
                + string(CThread::getName()) + " interrupted");
         // There is no need to break. Next is EWOULDBLOCK
      case EWOULDBLOCK:
         //Log("CUnixSocket::receive timeout");
         return ERR_OK;

      default:
         LOG4CPLUS_ERROR(logger, "CUnixSocket::receive():"
            + string(CThread::getName()) + " => ERR_CONNECTION_LOST "
            + convertIntegerToString(errno) + " " + strerror(errno));

         mSock = -1;

         return ERR_CONNECTION_LOST;
      }
   }

   mReceiveMutex.unlock();
}

void CUnixSocket::threadFunc()
{
   LOG4CPLUS_TRACE(logger, "CUnixSocket::threadFunc() this = "
                          + convertIntegerToString((intptr_t)this));
   UInt32 size = 0;

   assert(mpSubscriber);

   if (mIncomingListen)
   {
      assert(mSock != -1);

      timeval tv = { 0 };
      tv.tv_sec = 0;
      tv.tv_usec = 300;

      while (!getStopFlag())
      {
         int newSock = accept(mSock, NULL, NULL);
         assert(newSock != -1);
         LOG4CPLUS_INFO(logger, "CUnixSocket::threadFunc() new socket accepted this = "
                               + convertIntegerToString((intptr_t)this));
         mpSubscriber->onIncomingConnection(this, new CUnixSocket(false, mpSubscriber, newSock, mAskForReadiness));
         /*
         fd_set readfds;
         FD_ZERO(&readfds);
         FD_SET(mSock, &readfds);

         int ret = select(0, &readfds, NULL, NULL, &tv);
         if (ret > 0)
         {
            if (FD_ISSET(mSock, &readfds))
            {
               int newSock = accept(mSock, NULL, NULL);
               assert(newSock != -1);
               Log("CUnixSocket::threadFunc() new socket accepted this = %p", this);
               mpSubscriber->onIncomingConnection(this, new CUnixSocket(false, mpSubscriber, newSock));
            }
         }
         */
      }
   }
   else
   {
      ERROR_CODE err;
      bool canRecv = true;
      while (!getStopFlag())
      {
         if (mAskForReadiness)
         {
            err = receive(size, true);

            if (err == ERR_CONNECTION_LOST)
            {
               mpSubscriber->onConnectionLost(this);
               break;
            }
            else
            {
               mReceiveMutex.lock();

               if (size > 0)
               {
                  canRecv = mpSubscriber->isReadyToReceiveData(this, size);
                  if (!canRecv)
                     CThread::sleep(100);
               }
               else
                  canRecv = false;

               mReceiveMutex.unlock();
            }
         }

         if (canRecv)
         {
            if (receive(size) == ERR_CONNECTION_LOST)
            {
               mpSubscriber->onConnectionLost(this);
               break;
            }
            else if (!mDestroyed)
            {
               mReceiveMutex.lock();

               if (size > 0)
                  mpSubscriber->onReceive(this, mBuffer, size);

               mReceiveMutex.unlock();
            }
         }

      }
   }
}

