/* 
 * 
 * iviLINK SDK, version 1.1.2
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

   }

}
