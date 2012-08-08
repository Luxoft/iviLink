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









#include "CPmpCoreProfileInfo.hpp"
#include "utils/xml/pugixml.hpp"
#include "framework/components/ProfileManager/PMP/repository/CRepoController.hpp"

namespace iviLink
{

   namespace PMP
   {

      Logger CPmpCoreProfileInfo::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.CPmpCoreProfileInfo"));

      CPmpCoreProfileInfo::CPmpCoreProfileInfo()
         : mEnabledByComplement(false)
         , mEnabledByClient(true)
         , mLocked(false)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      CPmpCoreProfileInfo::CPmpCoreProfileInfo(const std::string & profileManifest)
      : mEnabledByComplement(false)
      , mEnabledByClient(true)
      , mLocked(false)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         parseXml(profileManifest);
      }

      CPmpCoreProfileInfo::CPmpCoreProfileInfo(const CPmpCoreProfileInfo &info)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mEnabledByClient = info.mEnabledByClient;
         mEnabledByComplement = info.mEnabledByComplement;
         mLocked = info.mLocked;
         mUid = info.mUid;
         mApi = info.mApi;
         mComplement = info.mComplement;
         mLibrary = info.mLibrary;
      }

      CPmpCoreProfileInfo& CPmpCoreProfileInfo::operator = (const CPmpCoreProfileInfo &info)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mEnabledByClient = info.mEnabledByClient;
         mEnabledByComplement = info.mEnabledByComplement;
         mLocked = info.mLocked;
         mUid = info.mUid;
         mApi = info.mApi;
         mComplement = info.mComplement;
         mLibrary = info.mLibrary;
         return *this;
      }

      void CPmpCoreProfileInfo::parseXml(const std::string & profileManifest)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         pugi::xml_document doc;
         pugi::xml_parse_result res = doc.load(profileManifest.c_str());
         if(pugi::status_ok != res.status)
         {
            LOG4CPLUS_INFO(msLogger, "Error while parsing Profile API manifest");
            return;
         }

         pugi::xml_node prof = doc.child("profile");
         mUid = Profile::Uid(prof.child_value("uid"));
         mApi = Profile::ApiUid(prof.child("api").attribute("uid").value());
         mComplement = Profile::Uid(prof.child_value("complement"));
      }

      void CPmpCoreProfileInfo::setLibrary(const std::string & library)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mLibrary = library;
      }

      CPmpCoreProfileInfo::~CPmpCoreProfileInfo()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      Profile::Uid CPmpCoreProfileInfo::uid() const
      {
         return mUid;
      }

      Profile::ApiUid CPmpCoreProfileInfo::api() const
      {
         return mApi;
      }

      Profile::Uid CPmpCoreProfileInfo::complement() const
      {
         return mComplement;
      }

      std::string CPmpCoreProfileInfo::library() const
      {
         LOG4CPLUS_INFO(msLogger, "mLibrary = " + mLibrary);
         return mLibrary;
      }

      bool CPmpCoreProfileInfo::locked() const
      {
         return mLocked;
      }

      bool CPmpCoreProfileInfo::enabled() const
      {
         return mEnabledByClient && mEnabledByComplement;
      }

      bool CPmpCoreProfileInfo::available() const
      {
         return enabled() && !mLocked;
      }

      void CPmpCoreProfileInfo::lock()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mLocked = true;
      }

      void CPmpCoreProfileInfo::unlock()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mLocked = false;
      }

      void CPmpCoreProfileInfo::enableByClient()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mEnabledByClient = true;
      }

      void CPmpCoreProfileInfo::disableByClient()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mEnabledByClient = false;
      }

      void CPmpCoreProfileInfo::disableByComplement()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mEnabledByComplement = false;
      }

      void CPmpCoreProfileInfo::enableByComplement()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mEnabledByComplement = true;
      }

   }

}
