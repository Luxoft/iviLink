#include "GlobalJavaVmGetter.h"
#include "BluetoothObject.h"

#include <jni.h>
#include <cassert>
#include <android/log.h>

jobject gBlueHelper;
jobject getBluetoothHelperInstance()
{
    __android_log_print(ANDROID_LOG_INFO, "iviLink.ConnectivityAgent.Android.BluetoothStorage", "BluetoothHelper requested!");
    return gBlueHelper;
};

void storeObject(jobject obj)
{
   __android_log_print(ANDROID_LOG_INFO, "iviLink.ConnectivityAgent.Android.BluetoothStorage", "ENTER: storeObject");
   gBlueHelper = obj;  
   __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "BluetoothHelper object stored successfully!");
   __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "EXIT: storeObject");
}



JavaVM * gJM;
JavaVM * globalGetJavaVM()
{
    __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "gJM requested!");
    return gJM;
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) 
{
    __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "ENTER: JNI_OnLoad");
    gJM = jvm;  
    assert(gJM);
    __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "gJM cached successfully!");
    __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "EXIT: JNI_OnLoad");
	return JNI_VERSION_1_6;
}
