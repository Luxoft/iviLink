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


#include "CRunApp.hpp"

namespace iviLink
{

   namespace AMP
   {

      Logger CRunApp::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.requestHandler.CRunApp"));

      CRunApp::CRunApp()
         : mAppId(-1)
         , mPid(-1)
         , mRegistred(false)
         , mNeedsSessionRequest(false)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      CRunApp::~CRunApp()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      void CRunApp::setId(int id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mAppId = id;
      }

      void CRunApp::setPid(pid_t pid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mPid = pid;
      }

      int CRunApp::id() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return mAppId;
      }

      pid_t CRunApp::pid() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return mPid;
      }

      void CRunApp::useService(const Service::Uid & service, bool use)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::string strUse =use ? std::string("true") : std::string("false");
         LOG4CPLUS_INFO(msLogger, "service: " + service.value() + "use : " + strUse);
         if (use)
         {
            mServices.insert(service);
         }
         else
         {
            mServices.erase(service);
         }
      }

      bool CRunApp::hasService(const Service::Uid & service) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::set<Service::Uid>::const_iterator it = mServices.find(service);
         return mServices.end() == it ? false : true;
      }

      void CRunApp::setSession(const Service::SessionUid & session)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mSession = session;
      }

      const Service::SessionUid & CRunApp::session() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return mSession;
      }

      void CRunApp::print() const
      {
         LOG4CPLUS_INFO(msLogger, "  AppID: " + convertIntegerToString(mAppId) +
               " PID: " + convertIntegerToString(mPid) + " Session: " + mSession.value());
         std::string services;
         for (std::set<Service::Uid>::const_iterator it = mServices.begin();
               mServices.end() != it; ++it)
         {
            services += " ";
            services += it->value();
         }
         LOG4CPLUS_INFO(msLogger, "   Services:" + services);
      }

      const std::set<Service::Uid> & CRunApp::getServices() const
      {
         return mServices;
      }

      void CRunApp::registerApp()
      {
         mRegistred = true;
      }

      void CRunApp::sendSessionRequest(bool send)
      {
         mNeedsSessionRequest = send;
      }

      bool CRunApp::isRegistred() const
      {
         return mRegistred;
      }

      bool CRunApp::needSessionRequest() const
      {
         return mNeedsSessionRequest;
      }

   }

}
