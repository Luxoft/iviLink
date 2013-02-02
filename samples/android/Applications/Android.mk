LOCAL_PATH := $(call my-dir)
SRC_PREFIX := $(LOCAL_PATH)/../../../../src

include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkApplicationLibrary
LOCAL_SRC_FILES := ../../../iviLink/include_android/native/libiviLinkApplicationLibrary.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkApplicationLibraryShared
LOCAL_SRC_FILES := ../../../iviLink/include_android/native/libiviLinkApplicationLibraryShared.so
include $(PREBUILT_SHARED_LIBRARY)