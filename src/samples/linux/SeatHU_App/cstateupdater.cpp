/* 
 * 
 * iviLINK SDK, version 1.0
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





#include "cstateupdater.h"
#include "samples/linux/Profiles/ProfileProxy/CSeatHUProxy.hpp"

CStateUpdater::CStateUpdater(CSeatState * pCSeatState)
   : CApp(iviLink::Service::Uid(SERVICE_NAME))
   , mSeatHUCallbacks(this)
{
   mLogger = Logger::getInstance("SeatHUSample.CStateUpdater");
   LOG4CPLUS_INFO(mLogger, "CStateUpdater created");
   mpCSeatState = pCSeatState;
   registerProfileCallbacks(iviLink::Profile::ApiUid(PROFILE_API_NAME), &mSeatHUCallbacks);
   std::string msg = std::string("resigtered profile callbacks for api ")+std::string(PROFILE_API_NAME);
   LOG4CPLUS_INFO(mLogger, msg);
}


CStateUpdater::~CStateUpdater()
{
    mpCSeatState = NULL;
}

// from CApp
void CStateUpdater::initDone(iviLink::ELaunchInfo launcher)
{
   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      LOG4CPLUS_INFO(mLogger, "started by user");
      std::string msg = std::string("trying to load service ")+std::string(SERVICE_NAME);
      LOG4CPLUS_INFO(mLogger, msg);
      if (loadService(iviLink::Service::Uid(SERVICE_NAME)))
      {
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
      LOG4CPLUS_INFO(mLogger, "started by iviLink");
   }
   CSeatHUProxy * serverProxy = new CSeatHUProxy(iviLink::Service::Uid(SERVICE_NAME));
   mpCSeatState->setProxy(serverProxy);
}



//connected to qml functions requesting change of state

void CStateUpdater::driverHeaterRequest()
{    
    mpCSeatState->driverHeater();
}

void CStateUpdater::passHeaterRequest()
{
    mpCSeatState->passHeater();
}

void CStateUpdater::driverSeatRequest()
{
    mpCSeatState->driverSeat();
}

void CStateUpdater::passSeatRequest()
{
    mpCSeatState->passSeat();
}

void CStateUpdater::backLeftRequest()
{
    mpCSeatState->backLeft();
}

void CStateUpdater::backRightRequest()
{
    mpCSeatState->backRight();
}

void CStateUpdater::bottomUpRequest()
{
    mpCSeatState->bottomUp();
}

void CStateUpdater::bottomDownRequest()
{
    mpCSeatState->bottomDown();
}

void CStateUpdater::bottomLeftRequest()
{
    mpCSeatState->bottomLeft();
}

void CStateUpdater::bottomRightRequest()
{
    mpCSeatState->bottomRight();
}

void CStateUpdater::initLocalRequest()
{
    // can be either called by client or by timer in main.qml
    mpCSeatState->initialization();
}

//Callback for profile created on the other side;

//change the current visible seat request
void CStateUpdater::onTabRequest(EPersonEvent event)
{
    if (event == DRIVER)
    {
        driverSeatRequest();
    }
    else if (event == PASSENGER)
    {
        passSeatRequest();
    }
}

//change the seat heater request
void CStateUpdater::onHeaterRequest(EPersonEvent event)
{
    if (event == DRIVER)
    {
        driverHeaterRequest();
    }
    else if (event == PASSENGER)
    {
        passHeaterRequest();
    }
}

//change the settings of current visible seat request
void CStateUpdater::onMoveRequest(EMoveEvent event)
{
    if (event == BACK_LEFT)
    {
        backLeftRequest();
    }
    else if (event == BACK_RIGHT)
    {
        backRightRequest();
    }
    else if (event == BOTTOM_UP)
    {
        bottomUpRequest();
    }
    else if (event == BOTTOM_DOWN)
    {
        bottomDownRequest();
    }
    else if (event == BOTTOM_LEFT)
    {
        bottomLeftRequest();
    }
    else if (event == BOTTOM_RIGHT)
    {
        bottomRightRequest();
    }
}

//initialization state request
void CStateUpdater::onInitRequest()
{
    initLocalRequest();
}
