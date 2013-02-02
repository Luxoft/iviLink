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
#ifndef __C_APP_IMPLEMENTATION_HPP
#define __C_APP_IMPLEMENTATION_HPP

#include <jni.h>

#include "Application.hpp"
#include "AppLibCommon.hpp"
#include "AppInfo.hpp"
#include "Logger.hpp"

class CAppImplementation: public iviLink::Application
{
public:
    CAppImplementation(iviLink::Android::AppInfo appInfo,
            const iviLink::Service::ListOfUids &services, jobject appExternal);
    virtual ~CAppImplementation()
    {
    }

    // callbacks to be passed to java CApp
    virtual void onInitDone(iviLink::ELaunchInfo launcher);

    virtual void onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service);

    virtual void onIncomingServiceAfterLoading(const iviLink::Service::Uid &service);

    virtual void onServiceDropped(const iviLink::Service::Uid &service);

    virtual void onServiceLoadError(const iviLink::Service::Uid &service);

    virtual void onLinkUp();

    virtual void onPhysicalLinkDown();

private:
    void callStringVoidMethod(const std::string& param, jmethodID methodID);

private:
    iviLink::Android::AppInfo mAppInfo;
    jobject mjAppJava;
    jmethodID midInitDone;
    jmethodID midIncomingServiceBeforeLoading;
    jmethodID midIncomingServiceAfterLoading;
    jmethodID midServiceLoadError;
    jmethodID midServiceDropped;
    jmethodID midLinkUp;
    jmethodID midLinkDown;
    Logger mLog;
};

#endif //__C_APP_IMPLEMENTATION_HPP
