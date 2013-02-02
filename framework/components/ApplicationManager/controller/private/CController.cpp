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


#include "CController.hpp"
#include "CAppComThread.hpp"
#include "CLaunchThread.hpp"
#include "CRequestHandler.hpp"
#include "CAppManProtoServer.hpp"
#include "CPmpController.hpp"
#include "CDatabase.hpp"
#include "AmEventFactory.hpp"
#include "EventHandler.hpp"


namespace iviLink
{
   namespace AMP
   {

      Logger CController::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.controller.CController"));

      CController::CController(CDatabase * database, ILauncher * launcher)
         : mpAppComThread(new CAppComThread())
         , mpLaunchThread(new CLaunchThread())
         , mpAppManProtoServer(new AppMan::Ipc::CAppManProtoServer())
         , mpDatabase(database)
         , mpLauncher(launcher)
         , mpRequestHandler(new CRequestHandler())
         , mEventFactory(new AmEventFactory())
         , mEventHandler(new Utils::EventHandler())
      {
      }

      CController::~CController()
      {
         mEventHandler->deinit();
         delete mpAppComThread;
         delete mpLaunchThread;
         delete mpAppManProtoServer;
         delete mpDatabase;
         delete mpLauncher;
         delete mpRequestHandler;
         delete mEventFactory;
         delete mEventHandler;
      }

      void CController::init()
      {
         mEventFactory->init(mEventHandler, AppMan::Ipc::CPmpController::instance()->pmpHandler());
         mpAppComThread->start();
         AppMan::Ipc::CPmpController::instance();
         mpLaunchThread->start();

         mpDatabase->print();

         mpRequestHandler->initDatabase(mpDatabase);
         mpRequestHandler->initApp(mpAppManProtoServer);
         mpRequestHandler->initPmp(AppMan::Ipc::CPmpController::instance()->pmpHandler());
         mpRequestHandler->initLaunchThread(mpLaunchThread);
         mpRequestHandler->initAppComThread(mpAppComThread);
         mpRequestHandler->initEventFactory(mEventFactory);

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
         mEventHandler->init();
      }

      void CController::uninit()
      {
         mpAppComThread->stopRequests();
         mpLaunchThread->stopLaunching();

         // mpDatabase->save();

         AppMan::Ipc::CPmpController::instance()->uninit();
         mpAppManProtoServer->init(0);
         mpAppManProtoServer->initConnectionHandler(0);
         mpAppComThread->uninit();
         mpLaunchThread->uninit();
         mpLauncher->deinit();
         mpRequestHandler->uninit();
      }

       void CController::setLinkStateGetter( std::tr1::function<bool ()> getter )
       {
           mpAppManProtoServer->set_link_state_getter( getter );
       }

       void CController::onLinkUpNotify()
       {
           LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

           if( mpAppManProtoServer )
               mpAppManProtoServer->onLinkUpNotify();
       }

       void CController::onLinkDownNotify()
       {
           LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

           if( mpAppManProtoServer )
               mpAppManProtoServer->onLinkDownNotify();
       }

   } /* namespace AMP */
} /* namespace AXIS */
