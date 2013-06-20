#ifndef BLUETOOTH_OBJECT_ANDROID_H
#define BLUETOOTH_OBJECT_ANDROID_H
#include <jni.h>

extern jobject gBlueHelper;


jobject getBluetoothHelperInstance();

void storeObject(jobject obj);

#endif // BLUETOOTH_OBJECT

