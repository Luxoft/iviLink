LOCAL_PATH := $(call my-dir)

ROOT_PATH           := $(LOCAL_PATH)/../../../../../..
UTILS_INCL 	        := $(shell find $(ROOT_PATH)/framework/utils/ -type d)
INTERNAL_APP_LIB    := $(shell find $(ROOT_PATH)/iviLink/include/ -type d)
JSON                := $(shell find $(ROOT_PATH)/3rd_party/json -type d)

include $(CLEAR_VARS)

LOCAL_MODULE := devicemanager
LOCAL_STATIC_LIBRARIES :=  iviLinkApplicationLibrary
MY_FILES := $(wildcard $(LOCAL_PATH)/src/*.cpp)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(MY_FILES)

LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(INTERNAL_APP_LIB)
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/libraries/DeviceManager/
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/libraries/trust_list/
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(JSON)

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkApplicationLibrary
LOCAL_SRC_FILES := ../../../../../../iviLink/include_android/native/libiviLinkApplicationLibrary.a
include $(PREBUILT_STATIC_LIBRARY)
