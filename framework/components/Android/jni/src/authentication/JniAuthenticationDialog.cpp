#include "JniAuthenticationDialog.hpp"
#include "JniThreadHelper.hpp"

namespace iviLink
{
namespace Authentication
{

using namespace iviLink::Android;
JniAuthenticationDialog::JniAuthenticationDialog(JavaVM* jm, jobject callbacks)
:
    mLogger(Logger::getInstance("JniAuthenticationDialog"))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mJM = jm;
    JniThreadHelper jth(mJM);
    JNIEnv* env = jth.getEnv();
    mCallbacks = env->NewGlobalRef(callbacks);
}

JniAuthenticationDialog::~JniAuthenticationDialog()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    JniThreadHelper jth(mJM);
    JNIEnv* env = jth.getEnv();
    env->DeleteGlobalRef(mCallbacks);
}

void JniAuthenticationDialog::showNumPad()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("showNumPad");
}

void JniAuthenticationDialog::hideNumPad()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("hideNumPad");
}

void JniAuthenticationDialog::showRetryPopup()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("showRetryPopup");
}

void JniAuthenticationDialog::resetNumPad()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("resetNumPad");
}


void JniAuthenticationDialog::lockInput()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("lockInput");
}
void JniAuthenticationDialog::unlockInput()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    callJavaMethod("unlockInput");
}

void JniAuthenticationDialog::callJavaMethod(const char * methodName)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Invoking: " + (std::string)methodName);
    JniThreadHelper jth(mJM);
    JNIEnv* env = jth.getEnv();
    jclass claz = env->GetObjectClass(mCallbacks);
    jmethodID mid = env->GetMethodID(claz, methodName, "()V");
    env->CallVoidMethod(mCallbacks, mid);
}

}
}