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


#include "ProfileInternal.hpp"
#include "PIUIDGenerator.hpp"

#include "CComponentMgr.hpp"
#include "IPMALIpcToPIM.hpp"

using namespace iviLink::PMAL;
using iviLink::Profile::IUid;

static const char gModuleName[] = "PMAL_PIUIDGenerator";

BaseError iviLink::PMAL::PIM::getPIUID(IUid& uid)
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMAL.PIUIDGenerator"));
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   if (!mgr)
      return BaseError(1, gModuleName, BaseError::IVILINK_ERROR);

   IPMALIpcToPIM* ipc = mgr->getIpcToPIM();

   bool res = ipc->generatePIUID(uid);
   return res ? CPMALError::NoPMALError(gModuleName) : CPMALError(CPMALError::ERROR_OTHER);
}

BaseError iviLink::PMAL::PIM::releasePIUID(IUid const& uid)
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMAL.PIUIDGenerator"));
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   if (!mgr)
      return BaseError(1, gModuleName, BaseError::IVILINK_ERROR);

   IPMALIpcToPIM* ipc = mgr->getIpcToPIM();

   if (!ipc)
      return BaseError(1, gModuleName, BaseError::IVILINK_ERROR);

   bool res = ipc->releasePIUID(uid);
   return res ? CPMALError::NoPMALError(gModuleName) : CPMALError(CPMALError::ERROR_OTHER);
}
