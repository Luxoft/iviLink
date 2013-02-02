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


#include <map>

#include "CLaunchThread.hpp"
#include "CSignalSemaphore.hpp"
#include "CMutex.hpp"

namespace iviLink
{

   namespace AMP
   {

      Logger CLaunchThread::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.requestHandler.CLaunchThread"));

      CLaunchThread::CLaunchThread()
         : CThread("CLaunchThread")
         , mpLauncher(0)
         , mBe(true)
         , mpSignalSemaphore(new CSignalSemaphore())
         , mpDatabase(0)
         , mpReqHandler(0)
         , mpSessionMutex(new CMutex())
         , mpReqMutex(new CMutex())
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      CLaunchThread::~CLaunchThread()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         delete mpSignalSemaphore;
         delete mpSessionMutex;
         delete mpReqMutex;
      }

      void CLaunchThread::initLauncher(ILauncher * pLauncher)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpLauncher = pLauncher;
         mpSignalSemaphore->signal();
      }

      void CLaunchThread::initDb(IDatabase * pDatabase)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpDatabase = pDatabase;
         mpSignalSemaphore->signal();
      }

      void CLaunchThread::initRequestHandler(IReqHandlerForLauncher * pReqHandler)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpReqHandler = pReqHandler;
         mpSignalSemaphore->signal();
      }

      void CLaunchThread::uninit()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         initDb(0);
         initLauncher(0);
         initRequestHandler(0);
      }

      void CLaunchThread::threadFunc()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         while (mBe)
         {
            if (!mpLauncher || !mpDatabase || !mpReqHandler || !hasRequests())
            {
               mpSignalSemaphore->wait();
            }
            else
            {
               handleRequest();
            }
         }
      }

      void CLaunchThread::stopLaunching()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mBe = false;
         mpSignalSemaphore->signal();
      }

      void CLaunchThread::launchApp(iviLink::Service::SessionUid session, int dbId)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpReqMutex->lock();
         mRequests.push_back(std::pair<Service::Uid, int>(session, dbId));
         mpReqMutex->unlock();
         mpSignalSemaphore->signal();
      }

      bool CLaunchThread::hasRequests()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpReqMutex->lock();
         bool res = !mRequests.empty();
         mpReqMutex->unlock();
         return res;
      }

      void CLaunchThread::handleRequest()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpReqMutex->lock();
         if (mRequests.empty())
         {
            mpReqMutex->unlock();
         }
         else
         {
            std::pair<Service::Uid, int> request = mRequests.front();
            mRequests.erase(mRequests.begin());
            mpReqMutex->unlock();
            std::string launchInfo;
            mpDatabase->appLaunchInfo(request.second,launchInfo);
            LOG4CPLUS_INFO(msLogger, "handleRequest():: launch info from database: " + launchInfo );
            if (launchInfo == "")
            {
               return;
            }

            mpSessionMutex->lock();
            mSessions[launchInfo] = request.first;
            mpSessionMutex->unlock();
            mpLauncher->launchApplication(launchInfo);
         }
      }

      void CLaunchThread::launchedApp(const std::string & launchInfo, pid_t pid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpSessionMutex->lock();
         std::map<std::string, Service::SessionUid>:: iterator it = mSessions.find(launchInfo);
         if (mSessions.end() == it)
         {
            mpSessionMutex->unlock();
            return;
         }
         Service::SessionUid session = it->second;
         mSessions.erase(it);
         mpSessionMutex->unlock();
         mpReqHandler->setSessionPid(session,pid);
      }

      void CLaunchThread::launchError(const std::string & launchInfo)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpSessionMutex->lock();
         std::map<std::string, Service::SessionUid>:: iterator it = mSessions.find(launchInfo);
         if (mSessions.end() == it)
         {
            mpSessionMutex->unlock();
            return;
         }
         Service::SessionUid session = it->second;
         mSessions.erase(it);
         mpSessionMutex->unlock();
         mpReqHandler->setSessionPid(session,-1);
      }

   }

}
