/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#ifndef CPMPCOMPONENTMANAGER_HPP_
#define CPMPCOMPONENTMANAGER_HPP_

#include <sys/types.h>

#include "Logger.hpp"

namespace iviLink
{

   namespace conf 
   {
      class Configurator;
   }  // namespace conf

   namespace AppMan
   {
      class IPmpHandler;
      class IPmpRequest;
   }  //namespace AppMan

   namespace PMP
   {
      namespace Ipc
      {
         class CPMPIpcProtocol;
      }  // namespace Ipc

      class CPmpCore;
      class CPmpCoreProtocol;
      class IPmpCoreToSysCtrl;
      class CPmpPim;
      class CPmpPimProtocol;
      class CPmpProtoController;

      /**
       * Class is used for managing PMP components. It's responsible for initializing and de-initializing
       * of PMP and connecting submodules with each other. It's singleton class.
       */
      class CPmpComponentManager
      {
      public:
         /**
          * @return instance of CPmpComponentManager
          */
         static CPmpComponentManager * instance(conf::Configurator * pConfig = 0);

         iviLink::AppMan::IPmpRequest * applicationManager();

         void initApplicationManagerHandler(iviLink::AppMan::IPmpHandler * pHandler);

         IPmpCoreToSysCtrl * getPmpCoreToSysCtrl();

      private:
         /**
          * Constructor
          */
         CPmpComponentManager(conf::Configurator * pConfig = 0);

         /**
          * Destructor
          */
         ~CPmpComponentManager();

         /**
          * Inits Profile repository
          * @retval true is success
          * @retval false if any problems
          */
         bool initProfileRepository();

         /**
          * Kills Profile Repository Process
          */
         void killProfileRepository();

         /**
          * Inits PMP components
          */
         void initPmp();

         /**
          * Uninits PMP components
          */
         void uninitPmp();

         CPmpProtoController* mpPmpProtoController;

         pid_t mRepoPid;                     ///< PID of Profile Repository Process
         CPmpCoreProtocol * mpCoreProtocol;  ///< Pointer to PMP Core Protocol instance
         CPmpCore * mpCore;                  ///< Pointer to Core instance
         CPmpPimProtocol * mpPimProtocol;    ///< Pointer to PMP PIM Protocol instance
         CPmpPim * mpPim;                    ///< Pointer to PIM instance
         iviLink::PMP::Ipc::CPMPIpcProtocol * mpIpcProtocol; ///< Pointer to IPC Protocol instance
         iviLink::conf::Configurator * mpConfig;

         static Logger msLogger; ///< object of logger
      };

   }  // namespace PMP
}  // namespace AXIS

#endif /* CPMPCOMPONENTMANAGER_HPP_ */
