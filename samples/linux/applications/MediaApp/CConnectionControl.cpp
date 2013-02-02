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
#include <iostream>
#include <cassert>
#include <cstdlib>

#include "Exit.hpp"

#include "CConnectionControl.h"
#include "CMediaSourceClientProfileProxy.hpp"
#include "CMediaSourceServerProfileProxy.hpp"
#include "CMediaControlClientProfileProxy.hpp"
#include "CMediaControlServerProfileProxy.hpp"
#include "CMediaStreamingClientProfileProxy.hpp"
#include "CMediaStreamingServerProfileProxy.hpp"


Logger CConnectionControl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.Media"));

CConnectionControl::CConnectionControl(IGuiController * controller, IMediaSource * source, IPlayer * player, IServer * server, iviLink::Android::AppInfo info)
:
    Application(iviLink::Service::Uid("MediaService"), info),
    mAppInfo(info),
    mCBregistered(false),
    mGuiController(controller)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPlayerLogic = new PlayerLogic(mAppInfo, controller, source, player, server);
}

void CConnectionControl::init()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   initInIVILink();
}

CConnectionControl::~CConnectionControl()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mPlayerLogic)
        delete mPlayerLogic;
}

PlayerLogic* CConnectionControl::getPlayerLogic()
{
    return mPlayerLogic;
}

void CConnectionControl::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    registerMediaCallbacks();
    mPlayerLogic->initByLauncher();
    mGuiController->lockGUI();
}

void CConnectionControl::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mGuiController->unlockGUI();
    mPlayerLogic->connection();
}

void CConnectionControl::onServiceDropped(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    killProcess();
}

void CConnectionControl::onInitDone(iviLink::ELaunchInfo launcher)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (iviLink::LAUNCHED_BY_USER == launcher)
    {
        LOG4CPLUS_INFO(msLogger, "started by user");
        mPlayerLogic->initByUser();
        registerMediaCallbacks();
        mGuiController->lockGUI();
        iviLink::Error loadError = loadService(iviLink::Service::Uid("MediaService"));
        if(loadError.isNoError())
        {
            LOG4CPLUS_INFO(msLogger, "MediaService started");
            mGuiController->unlockGUI();
            mPlayerLogic->connection();
        } else
        {
            LOG4CPLUS_ERROR(msLogger, "MediaService NOT started: " + loadError.toString());
            killProcess(1);
        }
    }
    else
    {
        LOG4CPLUS_INFO(msLogger, "started by iviLink");
        registerMediaCallbacks();
        mPlayerLogic->initByLauncher();
    }


}

void CConnectionControl::onServiceLoadError(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_FATAL(msLogger, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
   LOG4CPLUS_FATAL(msLogger, "service load error: " + service.value());
   LOG4CPLUS_FATAL(msLogger, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
   killProcess(1);
}

void CConnectionControl::registerMediaCallbacks()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (!mCBregistered)
    {
        registerProfileCallbacks(iviLink::Profile::ApiUid("MediaSourceServerProfile_API_UID"), static_cast<IMediaSourceServerProfile_API::Callbacks*>( mPlayerLogic ));
        registerProfileCallbacks(iviLink::Profile::ApiUid("MediaSourceClientProfile_API_UID"), static_cast<IMediaSourceClientProfile_API::Callbacks*>(mPlayerLogic));
        registerProfileCallbacks(iviLink::Profile::ApiUid("MediaControlServerProfile_API_UID"),static_cast<IMediaControlServerProfile_API::Callbacks*> (mPlayerLogic));
        registerProfileCallbacks(iviLink::Profile::ApiUid("MediaControlClientProfile_API_UID"), static_cast<IMediaControlClientProfile_API::Callbacks*>(mPlayerLogic));
        registerProfileCallbacks(iviLink::Profile::ApiUid("MediaStreamingServerProfile_API_UID"), static_cast<IMediaStreamingServerProfile_API::Callbacks*>(mPlayerLogic));
        registerProfileCallbacks(iviLink::Profile::ApiUid("MediaStreamingClientProfile_API_UID"), static_cast<IMediaStreamingClientProfile_API::Callbacks*>(mPlayerLogic));
        mCBregistered = true;
    }
}
