#include "JniDeviceManagerDialog.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"
#include "DeviceManager.hpp"

namespace iviLink
{
namespace DeviceManager
{

using namespace iviLink::Android;
JniDeviceManagerDialog::JniDeviceManagerDialog(JavaVM* jm, jobject callbacks)
:
    mLogger(Logger::getInstance("JniDeviceManagerDialog"))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mJM = jm;
    JniThreadHelper jth(mJM);
    JNIEnv* env = jth.getEnv();
    mCallbacks = env->NewGlobalRef(callbacks);
}

JniDeviceManagerDialog::~JniDeviceManagerDialog()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    JniThreadHelper jth(mJM);
    JNIEnv* env = jth.getEnv();
    env->DeleteGlobalRef(mCallbacks);
}

void JniDeviceManagerDialog::setDeviceManager(DeviceManager* deviceManager)
{
    mDeviceManager=deviceManager;
}

void JniDeviceManagerDialog::disconnectDevice(int index)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    mDeviceManager->disconnectDevice(index);
}

void JniDeviceManagerDialog::updateListContent(CTrustList::tNameUidVector trustList)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    JniThreadHelper jth(mJM);
    JNIEnv* env = jth.getEnv();
    jclass claz = env->GetObjectClass(mCallbacks);
    jmethodID mid = env->GetMethodID(claz, "updateListContent", "(Ljava/lang/String;)V");

    for(int i = 0; i < trustList.size(); i++)
    {
        jstring item;
        if(trustList[i].second != "")
        {
            item = iviLink::Android::StringConversion::stdToJ(trustList[i].second, env);
        }
        else
        {
            item = iviLink::Android::StringConversion::stdToJ(trustList[i].first.value(), env);
        }
        env->CallVoidMethod(mCallbacks, mid, item);
    }
}

void JniDeviceManagerDialog::callJavaMethod(const char * methodName)
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
