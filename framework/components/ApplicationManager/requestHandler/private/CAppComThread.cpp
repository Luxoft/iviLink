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


#include "CAppComThread.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"

namespace iviLink
{

   namespace AMP
   {

      Logger CAppComThread::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.requestHandler.CAppComThread"));

      CAppComThread::CAppComThread()
         : CThread("CAppComThread")
         , mpPmp(0)
         , mBe(true)
         , mpReqMutex(new CMutex())
         , mpSignalSemaphore(new CSignalSemaphore())
         , mpApp(0)
         , mpHandlerForAppCom(0)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      CAppComThread::~CAppComThread()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         delete mpSignalSemaphore;
         delete mpReqMutex;
      }

      void CAppComThread::initPmpHandler(AppMan::IPmpHandler * pHandler)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpPmp = pHandler;
         mpSignalSemaphore->signal();
      }

      void CAppComThread::initAppHandler(AppMan::Ipc::IAppManProtoAmpToApp * pApp)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpApp = pApp;
         mpSignalSemaphore->signal();
      }

      void CAppComThread::initAppComHandler(IReqHandlerForAppCom * pHandlerForAppCom)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpHandlerForAppCom = pHandlerForAppCom;
         mpSignalSemaphore->signal();
      }

      void CAppComThread::uninit()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         initPmpHandler(0);
         initAppHandler(0);
         initAppHandler(0);
      }

      void CAppComThread::stopRequests()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mBe = false;
         mpSignalSemaphore->signal();
      }

      void CAppComThread::addRequest(iviLink::Service::SessionUid session)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpReqMutex->lock();
         mRequests.push_back(session);
         mpReqMutex->unlock();
         mpSignalSemaphore->signal();
      }

      void CAppComThread::addSessionRequest(pid_t pid, const iviLink::Service::SessionUid & session,
            const iviLink::Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpReqMutex->lock();
         mSessionRequests[pid] = std::pair<Service::SessionUid, Service::Uid>(session,service);
         mpReqMutex->unlock();
         mpSignalSemaphore->signal();
      }

      void CAppComThread::addWaitingServiceRequest(pid_t pid, const iviLink::Service::Uid & service)
      {
          LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
          mpReqMutex->lock();
          mSessionRequests[pid] = std::pair<Service::SessionUid, Service::Uid>(Service::SessionUid(""),service);
          mpReqMutex->unlock();
          mpSignalSemaphore->signal();
      }

      void CAppComThread::addAppLaunchInfoRequest(pid_t pid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpReqMutex->lock();
         mAppLaunchRequests.push_back(pid);
         mpReqMutex->unlock();
         mpSignalSemaphore->signal();
      }

      void CAppComThread::threadFunc()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         while (mBe)
         {
            if (!mpPmp || ! hasRequests())
            {
               mpSignalSemaphore->wait();
            }
            else
            {
               handleRequest();
            }
         }
      }

      bool CAppComThread::hasRequests()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpReqMutex->lock();
         bool res = !mRequests.empty() || !mSessionRequests.empty() || !mAppLaunchRequests.empty();
         mpReqMutex->unlock();
         return res;
      }

      void CAppComThread::handleRequest()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (mpPmp)
         {
            mpReqMutex->lock();
            std::list<Service::SessionUid>::iterator it = mRequests.begin();
            if (mRequests.end() == it)
            {
               mpReqMutex->unlock();
            }
            else
            {
               Service::SessionUid session = (*it);
               mRequests.erase(it);
               mpReqMutex->unlock();
               if (session.value() != "")
               {
                   mpPmp->appRequestError(session);
               }
            }
         }
         if (mpApp)
         {
            {
               mpReqMutex->lock();
               std::map<pid_t, std::pair<Service::SessionUid, Service::Uid> >::iterator it =
                     mSessionRequests.begin();
               if (mSessionRequests.end() == it)
               {
                  mpReqMutex->unlock();
               }
               else
               {
                  pid_t pid = it->first;
                  std::pair<Service::SessionUid, Service::Uid> req = it->second;
                  mSessionRequests.erase(it);
                  mpReqMutex->unlock();
                  if (req.first.value() == "") //waiting service request
                  {
                      mpApp->waitingServiceRequest(pid, req.second);
                  }
                  else
                  {
                      mpApp->sessionRequest(pid, req.first, req.second);
                  }
               }
            }
            {
               mpReqMutex->lock();
               std::list<pid_t>::iterator it = mAppLaunchRequests.begin();
               if (mAppLaunchRequests.end() == it || ! mpHandlerForAppCom)
               {
                  mpReqMutex->unlock();
               }
               else
               {
                  pid_t pid = (*it);
                  mAppLaunchRequests.erase(it);
                  mpReqMutex->unlock();
                  std::string str;
                  mpApp->getAppLaunchInfo(pid, str);
                  mpHandlerForAppCom->setProcAppLanchInfo(pid,str);
               }
            }
         }
      }
   }

}
