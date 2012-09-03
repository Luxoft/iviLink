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









#include "CProfileInternal.hpp"
#include "PIUIDGenerator.hpp"

#include "framework/components/ProfileManager/PMAL/CComponentMgr.hpp"
#include "framework/components/ProfileManager/PMAL/ipc_protocol/IPMALIpcToPIM.hpp"

using namespace iviLink::PMAL;
using iviLink::Profile::IUid;

static const char gModuleName[] = "PMAL_PIUIDGenerator";


CError iviLink::PMAL::PIM::getPIUID(IUid& uid)
{
   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   if (!mgr)
      return CError(1, gModuleName, CError::ERROR);

   IPMALIpcToPIM* ipc = mgr->getIpcToPIM();

   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMAL.PIUIDGenerator"));
   CError err = ipc->generatePIUID(uid);
   if (!err.isNoError())
   {
      /// @todo error processing
      LOG4CPLUS_INFO(logger, "todo: error processing");
      return err;
   }

   return CError::NoError(gModuleName);
}

CError iviLink::PMAL::PIM::releasePIUID(IUid const& uid)
{
   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   if (!mgr)
      return CError(1, gModuleName, CError::ERROR);

   IPMALIpcToPIM* ipc = mgr->getIpcToPIM();

   if (!ipc)
      return CError(1, gModuleName, CError::ERROR);

   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMAL.PIUIDGenerator"));

   CError err = ipc->releasePIUID(uid);
   if (!err.isNoError())
   {
      /// @todo error processing
      LOG4CPLUS_INFO(logger, "todo: error processing");
      return err;
   }

   return CError::NoError(gModuleName);
}
