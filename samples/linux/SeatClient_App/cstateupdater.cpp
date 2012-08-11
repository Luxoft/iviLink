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





#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>

#include "samples/linux/Profiles/ProfileProxy/CSeatClientProxy.hpp"
#include "cstateupdater.h"

CStateUpdater::CStateUpdater(CRequestProcessor * pCRequestProcessor)
   : CApp(iviLink::Service::Uid(SERVICE_NAME))
   , mSeatClientCallbacks(this)
{
   isQmlReady = false;
   isInitDone = false;
   mLogger = Logger::getInstance("SeatClientSample.CStateUpdater");
   LOG4CPLUS_INFO(mLogger, "CStateUpdater created");
   mpRequestProcessor = pCRequestProcessor;
   registerProfileCallbacks(iviLink::Profile::ApiUid(PROFILE_API_NAME), &mSeatClientCallbacks);
   std::string msg = std::string("registered profile callbacks for api ")+std::string(PROFILE_API_NAME);
   LOG4CPLUS_INFO(mLogger, msg);
}

CStateUpdater::~CStateUpdater()
{
    if(mpRequestProcessor) delete mpRequestProcessor;
}

void CStateUpdater::initDone(iviLink::ELaunchInfo launcher)
{
   
   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      LOG4CPLUS_INFO(mLogger, "started by user");
      std::string msg = std::string("trying to load service ")+std::string(SERVICE_NAME);
      LOG4CPLUS_INFO(mLogger, msg);
      if (loadService(iviLink::Service::Uid(SERVICE_NAME)))
      {
         isInitDone = true;
         LOG4CPLUS_INFO(mLogger, "service started");
      }
      else
      {
         LOG4CPLUS_ERROR(mLogger, "service NOT started");
         return;
      }
   }
   else
   {
      isInitDone = true;
      LOG4CPLUS_INFO(mLogger, "started by iviLink");
   }
   CSeatClientProxy * clientProxy = new CSeatClientProxy(iviLink::Service::Uid(SERVICE_NAME));
   mpRequestProcessor->setProxy(clientProxy);
   if(isQmlReady)  
   {  
      LOG4CPLUS_INFO(mLogger, "GUI is ready, sending init request");
      mpRequestProcessor->initRequest(); //otherwise the request will be called when it is
   }
}


void CStateUpdater::onQmlVisible()
{
   LOG4CPLUS_INFO(mLogger, "got signal that GUI is ready");
   isQmlReady = true;
   if(isInitDone)
   {
      LOG4CPLUS_INFO(mLogger, "got signal that GUI is ready, init is done as well, sending init request");
      mpRequestProcessor->initRequest();
   }
}
         

//Callback for profile created on the other side; notification about seat state
void CStateUpdater::incomingNotification(Json::Value seatState)
{ 
    Json::StyledWriter writer;
    std::string deserializedJson = writer.write(seatState);
    LOG4CPLUS_INFO(mLogger, "The following state came: "+deserializedJson); 
    //updating GUI according to the new state 
    emit showSeat();
    int status = seatState.get("currSeat", 0).asInt();
    if (status == 0)
    {
        LOG4CPLUS_INFO(mLogger, "Currently displayed seat is Driver's seat"); 
        emit current_seat_viewDriver();
    }
    else
    {
        LOG4CPLUS_INFO(mLogger, "Currently displayed seat is Passenger's seat"); 
        emit current_seat_viewPass();
    }
    status = seatState.get("heaterDriver", 0).asInt();
    emit heaterDriver(status);
    status = seatState.get("heaterPass", 0).asInt();
    emit heaterPass(status);
    status = seatState.get("bottom_x", 0).asInt();
    emit bottom_x(status);
    status = seatState.get("bottom_y", 0).asInt();
    emit bottom_y(status);
    status = seatState.get("back_x", 0).asInt();
    emit back_x(status);
    status = seatState.get("back_y", 0).asInt();
    emit back_y(status);
    status = seatState.get("back_angle", 0).asInt();
    emit back_angle(status);
}

