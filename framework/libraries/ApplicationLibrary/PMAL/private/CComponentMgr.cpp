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


#include "CComponentMgr.hpp"
#include "CPIM.hpp"
#include "CPmalCore.hpp"
#include "CPMALIpcProtocol.hpp"

#include "configurator.h"

static const char gModuleName[] = "CComponentMgr";


namespace iviLink {
namespace PMAL {

Logger CPMALComponentMgr::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMAL.CPMALComponentMgr"));

class CPMALComponentMgr::CComponentMgrImpl
{
public:

   IPMALPIM* getPIM() const
   {
      return mpPim;
   }

   IPMALCore* getCore() const
   {
      return mpCore;
   }

   IPMALCoreToPIM* getCoreToPIM() const
   {
      return mpCore;
   }

   IPMALIpcToPIM* getIpcToPIM() const
   {
      return mpIpcProtocol;
   }

   IPMALPIMToIpc* getPIMToIpc() const
   {
      return mpPim;
   };

   IProfileManagerCallbacks* getProfileManagerCallbacks() const
   {
      return mpProfileManagerCallbacks;
   }

   AppMan::IAppManHandler* getAppManHandler() const
   {
      return mpPim;
   }

   CPMALError init(IProfileManagerCallbacks* pClbs, conf::Configurator* pConfig)
   {
      mpProfileManagerCallbacks = pClbs;
      mpConfig = pConfig;

      std::string pmpAddr;

      if (mpConfig)
      {
         pmpAddr = mpConfig->getParam("pmp_ipc_address");
      }

      mpIpcProtocol = new Ipc::CIpcProtocol(pmpAddr.empty() ? NULL : pmpAddr.c_str());

      BaseError err = mpIpcProtocol->connect();
      while(!err.isNoError())
      {
          err = mpIpcProtocol->connect();
          usleep(50000);
      }

      if (!err.isNoError())
      {
         delete mpIpcProtocol;
         mpIpcProtocol = NULL;

         LOG4CPLUS_ERROR(msLogger, "CComponentMgrImpl::init() " + static_cast<std::string>(err));

         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "ipc error");
      }

      mpCore = new CPmalCore(mpIpcProtocol);

      mpPim = new CPIM();


      return CPMALError::NoPMALError(gModuleName);
   }


public:

   CComponentMgrImpl() :
      mpConfig(NULL),
      mpProfileManagerCallbacks(NULL),
      mpPim(NULL),
      mpCore(NULL),
      mpIpcProtocol(NULL)      
   {

   }

   ~CComponentMgrImpl()
   {
      delete mpPim;
      mpPim = NULL;

      delete mpCore;
      mpCore = NULL;

      delete mpIpcProtocol;
      mpIpcProtocol = NULL;
   }

private:

   // obtained
   conf::Configurator* mpConfig;
   IProfileManagerCallbacks* mpProfileManagerCallbacks;

   // created
   CPIM* mpPim;
   CPmalCore* mpCore;
   Ipc::CIpcProtocol* mpIpcProtocol;
};


///

CPMALComponentMgr* CPMALComponentMgr::mpInstance = NULL;

CPMALComponentMgr* CPMALComponentMgr::getInstance()
{
   return mpInstance;
}

CPMALError CPMALComponentMgr::create(IProfileManagerCallbacks* pClbs, conf::Configurator* pConf/* = NULL*/)
{
   if (mpInstance)
      return CPMALError(CPMALError::ERROR_PMAL_ALREADY_INITIALIZED, gModuleName, "already created");

   mpInstance = new CPMALComponentMgr();
   if (!mpInstance)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "memory error");

   CPMALError err = mpInstance->mpImpl->init(pClbs, pConf);
   if (!err.isNoError())
   {
      delete mpInstance;
      mpInstance = NULL;
   }

   return err;
}

CPMALError CPMALComponentMgr::destroy()
{
   if (mpInstance)
   {
      delete mpInstance;
      mpInstance = NULL;
   }

   return CPMALError::NoPMALError(gModuleName);
}

///

IPMALPIM* CPMALComponentMgr::getPIM() const
{
   return mpImpl->getPIM();
}

IPMALCore* CPMALComponentMgr::getCore() const
{
   return mpImpl->getCore();
}

IPMALCoreToPIM* CPMALComponentMgr::getCoreToPIM() const
{
   return mpImpl->getCoreToPIM();
}

IPMALIpcToPIM* CPMALComponentMgr::getIpcToPIM() const
{
   return mpImpl->getIpcToPIM();
}

IPMALPIMToIpc* CPMALComponentMgr::getPIMToIpc() const
{
   return mpImpl->getPIMToIpc();
}

IProfileManagerCallbacks* CPMALComponentMgr::getProfileManagerCallbacks() const
{
   return mpImpl->getProfileManagerCallbacks();
}

iviLink::AppMan::IAppManHandler* CPMALComponentMgr::getAppManHandler() const
{
   return mpImpl->getAppManHandler();
}

///

CPMALComponentMgr::CPMALComponentMgr() :
      mpImpl(new CComponentMgrImpl)
{

}

CPMALComponentMgr::~CPMALComponentMgr()
{
   delete mpImpl;
}


}  // namespace ProfileManager
}  // namespace AXIS
