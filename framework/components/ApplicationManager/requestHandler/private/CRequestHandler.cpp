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

#include "CMutex.hpp"
#include "CRequestHandler.hpp"
#include "AmEventFactory.hpp"

namespace iviLink
{

   namespace AMP
   {

      Logger CRequestHandler::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.requestHandler.CRequestHandler"));

      CRequestHandler::CRequestHandler()
         : mpApp(0)
         , mpPmp(0)
         , mpDatabase(0)
         , mpAppMutex(new CMutex())
         , mpLaunchThread(0)
         , mpSessionMutex(new CMutex())
         , mpAppComThread(0)
         , mpSessionServicesMutex(new CMutex())
         , mEventFactory(NULL)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         printState();
      }

      CRequestHandler::~CRequestHandler()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         delete mpAppMutex;
         delete mpSessionMutex;
         delete mpSessionServicesMutex;
      }

      void CRequestHandler::initApp(AppMan::Ipc::IAppManProtoAmpToApp * pApp)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpApp = pApp;
      }

      void CRequestHandler::initPmp (AppMan::IPmpHandler * pPmp)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpPmp = pPmp;
      }

      void CRequestHandler::initDatabase(IDatabase * pDatabase)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpDatabase = pDatabase;
      }

      void CRequestHandler::initLaunchThread(ILaunchThread * pLaunchThread)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpLaunchThread = pLaunchThread;
      }

      void CRequestHandler::initAppComThread(IAppComThread * pAppComThread)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpAppComThread = pAppComThread;
      }

      void CRequestHandler::initEventFactory(AmEventFactory * eventFactory)
      {
          mEventFactory = eventFactory;
      }

      void CRequestHandler::uninit()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         initApp(0);
         initPmp(0);
         initDatabase(0);
         initLaunchThread(0);
         initAppComThread(0);
      }

//       IConnectionHandler
      void CRequestHandler::onConnectionLost(pid_t pid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpAppMutex->lock();
         std::map<pid_t, CRunApp>::iterator it = mApps.find(pid);
         if (mApps.end() != it)
         {
            mApps.erase(it);
         }
         mpAppMutex->unlock();
      }

//       IAppManProto
      AppMan::EInitResult CRequestHandler::initApplication(pid_t pid, std::list<Service::Uid> listOfSupportedServices)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "pid: " + convertIntegerToString(pid) + "Services: ");
         for (std::list<Service::Uid>::const_iterator it = listOfSupportedServices.begin();
               listOfSupportedServices.end() != it; ++it)
         {
            LOG4CPLUS_INFO(msLogger, "\t Service: " + (*it).value());
         }


         if (!mpDatabase || !mpAppComThread)
         {
            LOG4CPLUS_ERROR(msLogger, "CRequestHandler hasn't been inited");
            return AppMan::ERROR_AMP;
         }

         mpAppMutex->lock();
         mApps[pid].setPid(pid);
         std::map<pid_t, CRunApp>::iterator it = mApps.find(pid);
         if (mApps.end() == it)
         {
            mpAppMutex->unlock();
            printState();
            return AppMan::ERROR_AMP;
         }

         for (std::list<Service::Uid>::const_iterator i = listOfSupportedServices.begin();
               listOfSupportedServices.end() != i; ++i)
         {
            it->second.useService((*i),true);
         }
         int id = it->second.id();
         Service::SessionUid session = it->second.session();
         it->second.registerApp();
         LOG4CPLUS_INFO(msLogger, "after it->second.registerApp()");
         bool launchedByAppMan = false;
         if (it->second.needSessionRequest())
         {
            launchedByAppMan = true;
            mpAppMutex->unlock();
            mpSessionServicesMutex->lock();
            std::map<Service::SessionUid,Service::Uid>::iterator serviceIt =
                  mSessionServices.find(session);
            if (mSessionServices.end () != serviceIt)
            {
               mpAppComThread->addSessionRequest(it->second.pid(),it->second.session(),serviceIt->second);
            }
            mSessionServices.erase(serviceIt);
            mpSessionServicesMutex->unlock();
            mpAppMutex->lock();
            it = mApps.find(pid);
            if (mApps.end() != it)
            {
               it->second.sendSessionRequest(false);
            }
            mpAppMutex->unlock();
         }
         else
         {
            mpAppMutex->unlock();
         }

         LOG4CPLUS_INFO(msLogger, "app id in database: " + convertIntegerToString(id));

         if (id != -1)
         {
            for (std::list<Service::Uid>::const_iterator i = listOfSupportedServices.begin();
                  listOfSupportedServices.end() != i; ++i)
            {
               if (!mpDatabase->hasService(id,(*i)))
               {
                  mpDatabase->addService(id,(*i));
               }
            }
         }
         else
         {
            mpAppComThread->addAppLaunchInfoRequest(pid);
         }

         printState();
         return launchedByAppMan ? AppMan::STARTED_BY_APPMAN :  AppMan::STARTED_BY_USER;
      }

       bool CRequestHandler::isLinkAlive()
       {
           LOG4CPLUS_ERROR(msLogger, "CRequestHandler::is_link_alive(): this is dummy function, should not be called in normal situation");
           return false;
       }


      BaseError CRequestHandler::useService(pid_t pid, Service::Uid service, bool use)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpAppMutex->lock();
         mApps[pid];
         std::map<pid_t, CRunApp>::iterator it = mApps.find(pid);
         if (mApps.end() == it)
         {
            mpAppMutex->unlock();
            return BaseError::NoError("","");
         }

         it->second.useService(service,use);
         int id = it->second.id();
         mpAppMutex->unlock();

         if (id != -1 && mpDatabase)
         {
            if (!mpDatabase->hasService(id,service))
            {
               mpDatabase->addService(id,service);
            }
         }
         return BaseError::NoError("","");
      }

      BaseError CRequestHandler::registerService(pid_t pid, Service::Uid service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!mpDatabase)
         {
            return BaseError::NoError("AppMan","registerService No Database");
         }
         mpAppMutex->lock();
         std::map<pid_t, CRunApp>::iterator it = mApps.find(pid);
         if (mApps.end() == it)
         {
            return BaseError::NoError("AppMan", "Unknown PID");
         }
         int id = it->second.id();
         mpAppMutex->unlock();
         mpDatabase->addService(id,service);
         return BaseError::NoError("AppMan","registerService OK");
      }

//       IPmpConnectionStatus
      void CRequestHandler::onConnectionLost()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

//       IPmpRequest
      BaseError CRequestHandler::applicationRequest(const iviLink::Service::SessionUid & session,
              const iviLink::Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "Session: " + session.value() + ", Service: " + service.value());

         LOG4CPLUS_INFO(msLogger, "mApps size: " + convertIntegerToString(mApps.size()) +
         " mSessions size: " + convertIntegerToString(mSessions.size()) +
         "mSessionServices size: " + convertIntegerToString(mSessionServices.size()));

         if (!mpDatabase || !mpPmp || !mpLaunchThread)
         {
            LOG4CPLUS_WARN(msLogger, "AMP hasn't been inited!!!");
            return BaseError::NoError("","");
         }

         mpAppMutex->lock();
         for (std::map<pid_t, CRunApp>::iterator it = mApps.begin();
               mApps.end() != it; ++it)
         {
            LOG4CPLUS_INFO(msLogger, "find service: " + service.value());
            it->second.print();
            if (it->second.hasService(service))
            {
               LOG4CPLUS_INFO(msLogger, "has service " + service.value());
               it->second.setSession(session);
               LOG4CPLUS_INFO(msLogger, "first: " + convertIntegerToString(it->first) +
                       ", second.pid: " + convertIntegerToString(it->second.pid()));
               pid_t tempPid = it->first;
               mpAppMutex->unlock();
               mpAppComThread->addSessionRequest(tempPid,session,service);
               return BaseError::NoError();
            }
         }
         mpAppMutex->unlock();

         LOG4CPLUS_INFO(msLogger, "No running app that supports needed service");

         mpSessionMutex->lock();
         mSessions[session];
         std::map<Service::SessionUid, CRunApp>::iterator it = mSessions.find(session);
         if (mSessions.end() == it)
         {
            LOG4CPLUS_WARN(msLogger, "Session wasn't found");
            mpSessionMutex->unlock();
            printState();
            return BaseError::NoError("","");
         }
         it->second.sendSessionRequest(true);
         it->second.setSession(session);
         std::list<int> list;
         mpDatabase->getApps(service,list);
         if (list.empty())
         {
            LOG4CPLUS_WARN(msLogger, "list is empty");
            mpAppComThread->addRequest(session); // error, no app support this service
         }
         it->second.setId(list.front());
         mpSessionMutex->unlock();
         mpSessionServicesMutex->lock();
         mSessionServices[session] = service;
         mpSessionServicesMutex->unlock();
         mpLaunchThread->launchApp(session,list.front());

         return BaseError::NoError("","");
      }

      void CRequestHandler::waitingServiceReqeust(const iviLink::Service::Uid & service)
      {
          LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
          LOG4CPLUS_INFO(msLogger, "Service: " + service.value());


          if (!mpDatabase || !mpPmp || !mpLaunchThread)
          {
             LOG4CPLUS_WARN(msLogger, "AMP hasn't been inited!!!");
             return;
          }

          mpAppMutex->lock();
          for (std::map<pid_t, CRunApp>::iterator it = mApps.begin();
                mApps.end() != it; ++it)
          {
             it->second.print();
             if (it->second.hasService(service))
             {
                LOG4CPLUS_INFO(msLogger, "pid: " + convertIntegerToString(it->first) +
                        ", has service " + service.value());
                pid_t tempPid = it->first;
                mpAppMutex->unlock();
                mpAppComThread->addWaitingServiceRequest(tempPid, service);
                return;
             }
          }
          mpAppMutex->unlock();
          LOG4CPLUS_WARN(msLogger, "No app with needed services");
      }

      void CRequestHandler::setSessionPid(Service::SessionUid session, pid_t pid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "session: " + session.value() + ", pid: " + convertIntegerToString(pid));
         mpSessionMutex->lock();
         std::map<Service::SessionUid, CRunApp>::iterator it = mSessions.find(session);

         if(mSessions.end() == it)
         {
            LOG4CPLUS_INFO(msLogger, "mSessions.end() == it");
            mpSessionMutex->unlock();
         }
         else if (pid == -1)
         {
            LOG4CPLUS_INFO(msLogger, "pid == -1");
            mSessions.erase(it);
            mpSessionMutex->unlock();
            mpSessionServicesMutex->lock();
            mSessionServices.erase(session);
            mpSessionServicesMutex->unlock();
         }
         else
         {
            LOG4CPLUS_INFO(msLogger, "normal pid");
        	 if (!mpAppComThread)
        	 {
        	    LOG4CPLUS_WARN(msLogger, "!mpAppComThread");
             printState();
        		 return;
        	 }
            CRunApp app = it->second;
            mSessions.erase(it);
            mpSessionMutex->unlock();
            mpAppMutex->lock();
            mApps[pid].setPid(pid);
            mApps[pid].setId(app.id());
            mApps[pid].sendSessionRequest(app.needSessionRequest());
            mApps[pid].setSession(app.session());
            mpAppMutex->unlock();
            mpSessionServicesMutex->lock();////////////////////
            std::map<Service::SessionUid, Service::Uid>::const_iterator serIt = mSessionServices.find(session);
            if (mSessionServices.end() == serIt)
            {
               LOG4CPLUS_INFO(msLogger, "mSessionServices.end() == serIt");
               printState();
            	return;
            }
            Service::Uid service = serIt->second;
            mpSessionServicesMutex->unlock();
            mpAppComThread->addSessionRequest(pid, session, service);
            printState();
         }
      }

      void CRequestHandler::setProcAppLanchInfo(pid_t pid, const std::string &launchInfo)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!mpDatabase)
         {
        	 return;
         }
         int id = mpDatabase->getId(launchInfo);
         if (id == -1)
         {
            mpDatabase->addApp(launchInfo);
            id = mpDatabase->getId(launchInfo);
         }
         mpAppMutex->lock();
         std::map<pid_t, CRunApp>::iterator it = mApps.find(pid);
         if (mApps.end() == it)
         {
            mpAppMutex->unlock();
            return;
         }
         it->second.setId(id);
         std::set<Service::Uid> services = it->second.getServices();
         mpAppMutex->unlock();

         if (id != -1)
         {
            for (std::set<Service::Uid>::const_iterator it = services.begin();
                  services.end() != it; ++it)
            {
               if (!mpDatabase->hasService(id,(*it)))
               {
                  mpDatabase->addService(id,(*it));
               }
            }
         }
      }

      void CRequestHandler::printState() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );


         LOG4CPLUS_INFO(msLogger, "mApps: ");
         mpAppMutex->lock();
         for (std::map<pid_t, CRunApp>::const_iterator it = mApps.begin();
               mApps.end() != it; ++it)
         {
            LOG4CPLUS_INFO(msLogger, " App :: pid in map:" + convertIntegerToString(it->first));
            it->second.print();
         }
         mpAppMutex->unlock();

         LOG4CPLUS_INFO(msLogger, "mSessions :");
         mpSessionMutex->lock();
         for (std::map<Service::SessionUid, CRunApp>::const_iterator it = mSessions.begin();
               mSessions.end() != it; ++it)
         {
            LOG4CPLUS_INFO(msLogger, std::string(" ") + it->first.value() +
                  std::string(" pid: ") + convertIntegerToString(it->second.pid()) +
                  std::string("id: ") + convertIntegerToString(it->second.id()));
         }
         mpSessionMutex->unlock();

         LOG4CPLUS_INFO(msLogger, "mSessionServices:");
         mpSessionServicesMutex->lock();
         for (std::map<Service::SessionUid, Service::Uid>::const_iterator it = mSessionServices.begin();
               mSessionServices.end() != it; ++it)
         {
            LOG4CPLUS_INFO(msLogger, " " + it->first.value() + " : " + it->second.value());
         }
         mpSessionServicesMutex->unlock();
      }

    void CRequestHandler::onProfileManagerConnection()
    {
        LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
        usleep(500000);
        std::list<Service::Uid> services;
        mpAppMutex->lock();
        for(std::map<pid_t, CRunApp>::const_iterator it = mApps.begin();
                mApps.end() != it; ++it)
        {
            std::copy(it->second.getServices().begin(), it->second.getServices().end(),
                    back_inserter(services));
        }
        mpAppMutex->unlock();
        LOG4CPLUS_INFO(msLogger, "Service number: " + convertIntegerToString(services.size()));
        mEventFactory->waitingServicesRequest(services);
    }

}
}
