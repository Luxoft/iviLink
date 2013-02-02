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
#include <cstdlib>

#include "CPmalCore.hpp"
#include "pugixml.hpp"


namespace iviLink
{

   namespace PMAL
   {

      Logger CPmalCore::logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Pmal.Core"));

      CPmalCore::CPmalCore(IPMALIpcToCore * pIpc)
         : mpIpc(pIpc)
      {
          LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
          assert(pIpc);
      }

      CPmalCore::~CPmalCore()
      {
          LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
      }

      CPMALError CPmalCore::getProfileLibPath(Profile::Uid id, std::string& path)
      {
         LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
         bool res = mpIpc->getProfileLibPath(id,path);
         return res ? CPMALError::NoPMALError() : CPMALError(CPMALError::ERROR_OTHER);

      }

      CPMALError CPmalCore::isEnabledProfile(Profile::Uid id, bool& enabled)
      {
         LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
         std::map<std::string, std::string> params;
         std::list<iviLink::Profile::Uid> profiles;

         CPMALError err = findProfiles(id, params, profiles, true);
         enabled = !profiles.empty();
         return err;
         /// @todo: here we cannot distinguish whether profile is really
         /// disabled or just unknown
         /// So the method does not what its name suggests.
      }

      CPMALError CPmalCore::findProfiles(iviLink::BaseUid id,
            const std::map<std::string, std::string> & profileParameters
            , std::list<iviLink::Profile::Uid> & profiles, bool enabledProfiles/* = true*/)
      {
         LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
         profiles.clear();
         bool res = mpIpc->findProfiles(id, profileParameters, profiles, enabledProfiles);
         return res ? CPMALError::NoPMALError() : CPMALError(CPMALError::ERROR_OTHER);
      }

      CPMALError CPmalCore::disableByUid(iviLink::BaseUid id)
      {
         LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
         bool res = mpIpc->disableByUid(id);
         return res ? CPMALError::NoPMALError() : CPMALError(CPMALError::ERROR_OTHER);
      }

      CPMALError CPmalCore::enableByUid(iviLink::BaseUid id)
      {
         LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
         bool res = mpIpc->enableByUid(id);
         return res ? CPMALError::NoPMALError() : CPMALError(CPMALError::ERROR_OTHER);
      }

      CPMALError CPmalCore::enableAll()
      {
         LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
         bool res = mpIpc->enableAll();
         return res ? CPMALError::NoPMALError() : CPMALError(CPMALError::ERROR_OTHER);
      }

      CPMALError CPmalCore::getProfileInfo(iviLink::Profile::Uid id, ProfileInfo & profInfo)
      {
         LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
         std::string profileManifest;
         bool res = mpIpc->getManifest(id, profileManifest);
         profInfo.params.clear();
         if (!res)
         {
            profInfo.apiUid = BaseUid("");
            profInfo.complement = BaseUid("");
            profInfo.name = "";
            profInfo.uid = BaseUid("");
            profInfo.version = 0;
            return CPMALError(CPMALError::ERROR_OTHER);
         }

         pugi::xml_document doc;
         pugi::xml_parse_result pugiRes = doc.load(profileManifest.c_str());
         if(pugi::status_ok != pugiRes.status)
         {
            LOG4CPLUS_ERROR(logger, "CPmpCoreProfileInfo::parseXml() :: Error while parsing Profile manifest");
            return CPMALError(CPMALError::ERROR_MANIFEST_PARSING);
         }

         pugi::xml_node prof = doc.child("profile");
         profInfo.uid = iviLink::Profile::Uid(prof.child_value("uid"));
         profInfo.apiUid = iviLink::Profile::ApiUid(prof.child("api").attribute("uid").value());
         profInfo.name = prof.child_value("name");
         profInfo.version =  atoi(prof.child_value("version"));
         profInfo.complement = iviLink::Profile::Uid(prof.child_value("complement"));

         pugi::xml_node attrs = prof.child("attributes");

         for (pugi::xml_node_iterator it = attrs.begin(); it != attrs.end(); ++it)
         {
            if (it->name() == std::string("attribute"))
            {
               std::string atr = it->attribute("name").value();
               if (atr != "")
               {
                  profInfo.params[atr] = it->attribute("value").value();
               }
            }
         }

         return CPMALError::NoPMALError();
      }

   }

}
