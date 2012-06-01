LOCAL_PATH := $(call my-dir)

###### L0 Module ######
include $(CLEAR_VARS)
LOCAL_MODULE := L0

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SRC_FILES := source/CChannelAllocator.cpp
LOCAL_SRC_FILES += source/CTargetAgent.cpp
LOCAL_SRC_FILES += source/CSourceAgent.cpp
LOCAL_SRC_FILES += source/CProfileManager.cpp
LOCAL_SRC_FILES += source/CSystemControlProfile.cpp
LOCAL_SRC_FILES += source/CProfile.cpp

LOCAL_STATIC_LIBRARIES := LIB

LOCAL_LDLIBS := -llog

include $(BUILD_STATIC_LIBRARY)