#include "AndroidLauncher.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"

using namespace iviLink::Android;

namespace iviLink
{
namespace AMP
{

Logger AndroidLauncher::mLogger = Logger::getInstance("iviLink.AMP.Launcher.AndroidLauncher");

AndroidLauncher::AndroidLauncher(JavaVM * jm, jclass launcher)
{
    mJavaVM = jm;
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    mLaunchPerformer = (jclass)env->NewGlobalRef(launcher);
    mLaunchMethod = env->GetStaticMethodID(mLaunchPerformer, "startApplication","(Ljava/lang/String;)I");
}

void AndroidLauncher::init(ILauncherHandler * handler)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mInitedMutex);
    mHandler = handler;
}

void AndroidLauncher::launchApplication(const std::string & launchInfo)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mInitedMutex);
    if (mHandler)
    {
        JniThreadHelper jth(mJavaVM);
        JNIEnv * env = jth.getEnv();
        jstring param = StringConversion::stdToJ(launchInfo, env);
        pid_t pid=(pid_t)(env->CallStaticIntMethod(mLaunchPerformer, mLaunchMethod, param)); // int Launcher.startApplication(String) is invoked
        if(pid == -1)
        {
            LOG4CPLUS_FATAL(mLogger, "app not launched: "+launchInfo);
            mHandler->launchError(launchInfo);
            return;
        }
        LOG4CPLUS_INFO(mLogger, "app launched: ");
        LOG4CPLUS_INFO(mLogger, "launchInfo: "+launchInfo);
        LOG4CPLUS_INFO(mLogger, "pid: " + convertIntegerToString(pid));
        mHandler->launchedApp(launchInfo, pid);
    }
}

void AndroidLauncher::deinit()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mInitedMutex);
    mHandler = NULL;
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    env->DeleteGlobalRef(mLaunchPerformer);
}

}
}