LOCAL_PATH := $(call my-dir)
SRC_PREFIX := $(LOCAL_PATH)/../../../../src

include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkApplicationLibrary
LOCAL_SRC_FILES := ../../../../src/framework/libraries/AndroidPrebuilt/AndroidNativeAppLib/obj/local/armeabi/libiviLinkApplicationLibrary.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := json
LOCAL_SRC_FILES := ../../../../src/framework/libraries/AndroidPrebuilt/obj/local/armeabi/libJsonCpp.a
include $(PREBUILT_STATIC_LIBRARY)
