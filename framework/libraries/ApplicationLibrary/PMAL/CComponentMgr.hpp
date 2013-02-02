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


#ifndef PMAL_CCOMPONENTMGR_HPP_
#define PMAL_CCOMPONENTMGR_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "Logger.hpp"
#include "CPMALError.hpp"

namespace iviLink {
namespace AppMan {
class IAppManHandler;
}  // namespace AppMan

namespace conf {
class Configurator;
}  // namespace conf

namespace PMAL {

class IPMALPIM;
class IPMALCore;
class IPMALCoreToPIM;
class IPMALIpcToCore;
class IPMALIpcToPIM;
class IPMALPIMToIpc;
class IProfileManagerCallbacks;

/**
 * CComponentMgr class.
 * Class is designated to use by PMAL client and by other PMAL components.
 *
 * CComponentMgr :
 * * initializes and deinitializes PMAL;
 * * manages time of live of other PMAL components;
 * * provides the access to PMAL components through their interfaces.
 *
 * CComponentMgr is the singleton.
 */
class CPMALComponentMgr
{
public:

   /**
    * Returns the instance of the CComponentMgr singleton.
    * @retval pointer to CComponentMgr object
    * @retval NULL in case when CComponentMgr is not created yet
    */
   static CPMALComponentMgr* getInstance();

   /**
    * Creates the instance of CComponentMgr.
    * @param pCallbacks is the pointer to callback handler
    * @retval IVILINK_NO_ERROR PMAL initialized successfully
    * @retval ERROR_PMAL_ALREADY_INITIALIZED already initialized
    * @retval ERROR_PMP_CONNECTION unable to make connection to PMP
    * @retval ERROR_OTHER other error
    */
   static CPMALError create(IProfileManagerCallbacks* pCallbacks,
         conf::Configurator* pConf = NULL);

   /**
    * Destroys the current object of CComponentMgr.
    * Must be called by the PMAL client to release all obtained resources.
    * @retval IVILINK_NO_ERROR PMAL is destroyed successfully
    * @retval ERROR_OTHER
    */
   static CPMALError destroy();

   /**
    * Returns pointer to PIM interface of PMAL PIM
    */
   IPMALPIM* getPIM() const;

   /**
    * Returns pointer to IPMALCore interface of PMAL Core
    */
   IPMALCore* getCore() const;

   /**
    * Returns pointer to IPMALCoreToPIM interface of PMAL Core
    */
   IPMALCoreToPIM* getCoreToPIM() const;

   /**
    * Returns pointer to IPMALIpcToCore interface of PMAL Core
    */
   IPMALIpcToCore* getIpcToCore() const;

   /**
    * Returns pointer to IPMALIpcToPIM interface of PMAL PIM
    */
   IPMALIpcToPIM* getIpcToPIM() const;

   /**
    * Returns pointer to IPMALPIMToIpc interface of PMAL PIM
    */
   IPMALPIMToIpc* getPIMToIpc() const;

   /**
    * Returns pointer to IProfileManagerColbacks
    */
   IProfileManagerCallbacks* getProfileManagerCallbacks() const;

   /**
    * Returns pointer to IAppManHandler
    */
   iviLink::AppMan::IAppManHandler* getAppManHandler() const;

private:

   /**
    * Private constructor of singleton
    */
   CPMALComponentMgr();

   /**
    * Private destructor of singleton
    */
   ~CPMALComponentMgr();

   /**
    * Unimplemented copy constructor
    */
   CPMALComponentMgr(CPMALComponentMgr const&);

   /**
    * Unimplemented copy opeartor
    */
   CPMALComponentMgr& operator=(CPMALComponentMgr const&);

private:

   /**
    * For the testing purposes.
    */
   friend class CTestComponentMgr;

   /**
    * Forward declaration of impl class.
    */
   class CComponentMgrImpl;

   /**
    * Pointer to component manager implementation object.
    */
   CComponentMgrImpl* mpImpl;

   /**
    * Pointer to singleton instance.
    */
   static CPMALComponentMgr* mpInstance;

   static Logger msLogger; ///< object of logger
};


}  // namespace ProfileManager
}  // namespace AXIS


#endif /* PMAL_CCOMPONENTMGR_HPP_ */
