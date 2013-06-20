#include "AndroidStackRestarter.hpp"
#include "JniThreadHelper.hpp"

namespace iviLink
{

namespace SystemController
{

using namespace iviLink::Android;

AndroidStackRestarter::AndroidStackRestarter(JavaVM * jm, jobject watchdogWrapper)
{
    mJavaVM = jm;
    JniThreadHelper jth(mJavaVM);
    JNIEnv* env = jth.getEnv();
    mRestartPerformer = env->NewGlobalRef(watchdogWrapper);
}

void AndroidStackRestarter::restartIviLinkFromWatchdog()
{
    JniThreadHelper jth(mJavaVM);
    JNIEnv* env = jth.getEnv();
    jclass watchdogClaz = env->GetObjectClass(mRestartPerformer);
    jmethodID resetCallback = env->GetMethodID(watchdogClaz, "doReset", "()V");
    env->CallVoidMethod(mRestartPerformer, resetCallback);
}

AndroidStackRestarter::~AndroidStackRestarter()
{
    iviLink::Android::JniThreadHelper jth(mJavaVM);
    JNIEnv* env = jth.getEnv();
    env->DeleteGlobalRef(mRestartPerformer);
}

}
}