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
 

#ifndef CCONNECTIONCONTROL_H
#define CCONNECTIONCONTROL_H

#include "CApp.hpp"
#include "Logger.hpp"
#include "IGuiController.hpp"
#include "IMediaSource.hpp"
#include "IPlayer.hpp"
#include "IServer.hpp"

#include "CPlayerLogic.h"

#ifndef ANDROID
#else
#include "AppInfo.hpp"
#endif //ANDROID


class CConnectionControl : public iviLink::CApp

{
public:
#ifndef ANDROID
    CConnectionControl(IGuiController * controller, IMediaSource * source, IPlayer * player, IServer * server);
#else
    CConnectionControl(iviLink::Android::AppInfo info, IGuiController * controller, IMediaSource * source, IPlayer * player, IServer * server);
#endif //ANDROID
    virtual ~CConnectionControl();

    void init();

    // from CApp
    virtual void onInitDone(iviLink::ELaunchInfo launcher);
    virtual void onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service);
    virtual void onIncomingServiceAfterLoading(const iviLink::Service::Uid &service);
    virtual void onServiceDropped(const iviLink::Service::Uid &service);
    virtual void onServiceLoadError(const iviLink::Service::Uid &service);
    
    CPlayerLogic* getPlayerLogic();


private:

    CPlayerLogic*    mPlayerLogic;
    static Logger    msLogger;
    mutable bool     mCBregistered; 
    IGuiController * mGuiController;

#ifndef ANDROID
#else
    iviLink::Android::AppInfo mAppInfo;
#endif //ANDROID

    void registerMediaCallbacks();


};

#endif // CCONNECTIONCONTROL_H
