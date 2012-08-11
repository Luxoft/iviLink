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









#include "CController.hpp"
#include "framework/components/AppMan/requestHandler/CAppComThread.hpp"
#include "framework/components/AppMan/requestHandler/CLaunchThread.hpp"
#include "framework/components/AppMan/requestHandler/CRequestHandler.hpp"
#include "framework/libraries/AppMan/AmpForApp/CAppManProtoServer.hpp"
#include "framework/libraries/AppMan/AmpForPmp/CPmpController.hpp"
#include "framework/components/AppMan/database/CDatabase.hpp"
#include "framework/components/AppMan/launcher/CLauncher.hpp"


namespace iviLink
{
   namespace AMP
   {

      Logger CController::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.controller.CController"));

      CController::CController()
         : mpAppComThread(new CAppComThread())
         , mpLaunchThread(new CLaunchThread())
         , mpAppManProtoServer(new AppMan::Ipc::CAppManProtoServer())
         , mpDatabase(new CDatabase())
         , mpLauncher(new CLauncher())
         , mpRequestHandler(new CRequestHandler())
      {
      }

      CController::~CController()
      {
         delete mpAppComThread;
         delete mpLaunchThread;
         delete mpAppManProtoServer;
         delete mpDatabase;
         delete mpLauncher;
         delete mpRequestHandler;
      }

      void CController::init()
      {
         AppMan::Ipc::CPmpController::instance();
         mpAppComThread->start();
         mpLaunchThread->start();

         mpDatabase->load();
         mpDatabase->print();

         mpRequestHandler->initDatabase(mpDatabase);
         mpRequestHandler->initApp(mpAppManProtoServer,mpAppManProtoServer);
         mpRequestHandler->initPmp(AppMan::Ipc::CPmpController::instance()->pmpHandler());
         mpRequestHandler->initLaunchThread(mpLaunchThread);
         mpRequestHandler->initAppComThread(mpAppComThread);

         AppMan::Ipc::CPmpController::instance()->init(mpRequestHandler,mpRequestHandler);

         mpAppManProtoServer->init(mpRequestHandler);
         mpAppManProtoServer->initConnectionHandler(mpRequestHandler);

         mpAppComThread->initAppComHandler(mpRequestHandler);
         mpAppComThread->initAppHandler(mpAppManProtoServer);
         mpAppComThread->initPmpHandler(AppMan::Ipc::CPmpController::instance()->pmpHandler());

         mpLaunchThread->initDb(mpDatabase);
         mpLaunchThread->initLauncher(mpLauncher);
         mpLaunchThread->initRequestHandler(mpRequestHandler);

         mpLauncher->init(mpLaunchThread);

         mpAppManProtoServer->startWaitingForClients();

      }

      void CController::uninit()
      {
         mpAppComThread->stopRequests();
         mpLaunchThread->stopLaunching();

         mpDatabase->save();

         AppMan::Ipc::CPmpController::instance()->uninit();
         mpAppManProtoServer->init(0);
         mpAppManProtoServer->initConnectionHandler(0);
         mpAppComThread->uninit();
         mpLaunchThread->uninit();
         mpLauncher->init(0);
         mpRequestHandler->uninit();
      }

   } /* namespace AMP */
} /* namespace AXIS */
