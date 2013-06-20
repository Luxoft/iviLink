#include "CAppImplementation.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"

CAppImplementation::CAppImplementation(iviLink::Android::AppInfo appInfo,
        const iviLink::Service::ListOfUids &services, jobject appExternal) :
        Application(services, appInfo), mjAppJava(appExternal), mAppInfo(appInfo), mLog(
                Logger::getInstance("CAppImplementation"))
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    jclass claz = env->GetObjectClass(mjAppJava);
    midInitDone = env->GetMethodID(claz, "initDone", "(Z)V");
    midIncomingServiceBeforeLoading = env->GetMethodID(claz, "incomingServiceBeforeLoading",
            "(Ljava/lang/String;)V");
    midIncomingServiceAfterLoading = env->GetMethodID(claz, "incomingServiceAfterLoading",
            "(Ljava/lang/String;)V");
    midServiceDropped = env->GetMethodID(claz, "serviceDropped", "(Ljava/lang/String;)V");
    midServiceLoadError = env->GetMethodID(claz, "serviceLoadError", "(Ljava/lang/String;)V");
    midLinkUp = env->GetMethodID(claz, "connectionEstablished", "()V");
    midLinkDown = env->GetMethodID(claz, "connectionLost", "()V");
}

void CAppImplementation::onInitDone(iviLink::ELaunchInfo launcher)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    bool launchResult = (launcher == iviLink::LAUNCHED_BY_USER);
    env->CallVoidMethod(mjAppJava, midInitDone, static_cast<jboolean>(launchResult));
}

void CAppImplementation::callStringVoidMethod(const std::string& param, jmethodID methodID)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    jstring jParam = iviLink::Android::StringConversion::stdToJ(param, env);
    env->CallVoidMethod(mjAppJava, methodID, jParam);
}

void CAppImplementation::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    callStringVoidMethod(service.value(), midIncomingServiceBeforeLoading);
}

void CAppImplementation::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    callStringVoidMethod(service.value(), midIncomingServiceAfterLoading);
}

void CAppImplementation::onServiceDropped(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    callStringVoidMethod(service.value(), midServiceDropped);
}

void CAppImplementation::onServiceLoadError(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    callStringVoidMethod(service.value(), midServiceLoadError);
}

void CAppImplementation::onLinkUp()
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    env->CallVoidMethod(mjAppJava, midLinkUp);
}

void CAppImplementation::onPhysicalLinkDown()
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    env->CallVoidMethod(mjAppJava, midLinkDown);
}
