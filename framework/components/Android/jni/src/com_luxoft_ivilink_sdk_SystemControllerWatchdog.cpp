#include "com_luxoft_ivilink_sdk_SystemControllerWatchdog.hpp"
#include "SystemControllerWatchdogProcess.hpp"
#include "AndroidStackRestarter.hpp"

using namespace iviLink::SystemController;

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_SystemControllerWatchdog_startWD
  (JNIEnv * env, jobject watchdogWrapper)
{
    JavaVM * jm;
    env->GetJavaVM(&jm);
    AndroidStackRestarter * restarter = new AndroidStackRestarter(jm, watchdogWrapper);
    watchdogProcessEntryPoint(restarter);
    delete restarter;
}
