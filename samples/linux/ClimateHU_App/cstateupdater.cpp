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





#include "cstateupdater.h"
#include "samples/linux/Profiles/ProfileProxy/CClimateHUProxy.hpp"

CStateUpdater::CStateUpdater(CClimateCond * pCClimateCond)
   : CApp(iviLink::Service::Uid(SERVICE_NAME))
   , mpCClimateCond(pCClimateCond)
   , mClimateHUCallbacks(this)
{
   registerProfileCallbacks(iviLink::Profile::ApiUid(PROFILE_API_NAME), &mClimateHUCallbacks);
}

CStateUpdater::~CStateUpdater()
{
    mpCClimateCond = NULL;
}


void CStateUpdater::initDone(iviLink::ELaunchInfo launcher)
{
   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      loadService(iviLink::Service::Uid(SERVICE_NAME));
   }
}

void CStateUpdater::tempDecDrRequest()
{
    mpCClimateCond->tempDriverDown();
}

void CStateUpdater::tempIncDrRequest()
{
    mpCClimateCond->tempDriverUp();
}

void CStateUpdater::tempDecPassRequest()
{   
    mpCClimateCond->tempPassDown();
}

void CStateUpdater::tempIncPassRequest()
{   
    mpCClimateCond->tempPassUp();
}

void CStateUpdater::syncRequest()
{  
    mpCClimateCond->sync();
}

void CStateUpdater::fanLowRequest()
{ 
    mpCClimateCond->fanLow();
}

void CStateUpdater::fanMidRequest()
{ 
    mpCClimateCond->fanMid();
}

void CStateUpdater::fanHighRequest()
{
    mpCClimateCond->fanHigh();
}

void CStateUpdater::fanIncRequest()
{ 
    mpCClimateCond->fanInc();
}

void CStateUpdater::fanDecRequest()
{
    mpCClimateCond->fanDec();
}

void CStateUpdater::offRequest()
{ 
    mpCClimateCond->fanOff();
}

void CStateUpdater::rearRequest()
{ 
    mpCClimateCond->rear();
}

void CStateUpdater::frontRequest()
{  
    mpCClimateCond->front();
}

void CStateUpdater::acRequest()
{
    mpCClimateCond->ac();
}

void CStateUpdater::recircRequest()
{
    mpCClimateCond->recirc();
}

void CStateUpdater::autoRequest()
{
    mpCClimateCond->autoMode();
}

void CStateUpdater::initLocalRequest()
{ 
    mpCClimateCond->initialization();
}

//Callbacks for profile created on the other side;


//change temperature settings request
void CStateUpdater::onTempRequest(ETempEvent event)
{

    if (event == TEMP_UP_DRIVER)
    {
        tempIncDrRequest();
    }
    else if (event == TEMP_DOWN_DRIVER)
    {
        tempDecDrRequest();
    }
    else if (event == TEMP_UP_PASS)
    {
        tempIncPassRequest();
    }
    else if (event == TEMP_DOWN_PASS)
    {
        tempDecPassRequest();
    }
    else if (event == TEMP_SYNC)
    {
        syncRequest();
    }
}

//change fan settings request
void CStateUpdater::onFanRequest(EFanEvent event)
{
    if (event == FAN_LOW)
    {
        fanDecRequest();
    }
    else if (event == FAN_FAST)
    {
        fanIncRequest();
    }
    else if (event == FAN_OFF)
    {
        offRequest();
    }
}

//change heater settings request
void CStateUpdater::onHeaterRequest(EHeaterEvent event)
{
    if (event == HEAT_DOWN)
    {
        fanLowRequest();
    }
    else if (event == HEAT_ALL)
    {
        fanMidRequest();
    }
    else if (event == HEAT_UP)
    {
        fanHighRequest();
    }
}

//change general settings request
void CStateUpdater::onGeneralRequest(EGeneralRequest event)
{
    if (event == GEN_REQ_AC)
    {
        acRequest();
    }
    else if (event == GEN_REQ_AUTO)
    {
        autoRequest();
    }
    else if (event == GEN_REQ_REAR)
    {
        rearRequest();
    }
    else if (event == GEN_REQ_FRONT)
    {
        frontRequest();
    }
    else if (event == GEN_REQ_RECIR)
    {
        recircRequest();
    }
}

//initialization state request
void CStateUpdater::onInitRequest()
{
    initLocalRequest();
}


