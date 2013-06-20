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
