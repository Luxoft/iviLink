LOCAL_PATH := $(call my-dir)

ROOT_PATH       := $(LOCAL_PATH)/../../../../../..
UTILS_INCL 		:= $(shell find $(ROOT_PATH)/framework/utils/ -type d)
UTILS_INCL		+= $(ROOT_PATH)/framework/components/SystemController/componentLauncher
SAMPLES_INCL 	:= $(shell find $(ROOT_PATH)/framework/genericSamples/linux/ -type d)
PUBLIC_INCL     := $(shell find $(ROOT_PATH)/iviLink/include/ -type d)

include $(CLEAR_VARS)
LOCAL_MODULE := authenticationProfile
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary cryptoppRsa jsonCpp
MY_FILES := $(wildcard $(ROOT_PATH)/framework/genericSamples/linux/profiles/authentication/*.cpp)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(MY_FILES)

LOCAL_C_INCLUDES := $(UTILS_INCL)
LOCAL_C_INCLUDES += $(SAMPLES_INCL)
LOCAL_C_INCLUDES += $(PUBLIC_INCL)
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/libraries/trust_list/
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/libraries/trust_list/private

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := authenticationApp
LOCAL_STATIC_LIBRARIES := UdpSocket iviLinkApplicationLibrary
MY_FILES := $(wildcard $(LOCAL_PATH)/*.cpp)
MY_FILES += $(wildcard $(ROOT_PATH)/framework/genericSamples/linux/applications/authentication/*.cpp)
MY_FILES += $(wildcard $(ROOT_PATH)/framework/messageProtocols/SystemController_Authentication/Authentication/private/*.cpp)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(MY_FILES)

LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/messageProtocols/SystemController_Authentication/Authentication/
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/messageProtocols/SystemController_Authentication/
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/messageProtocols/
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/messageProtocols/WatchdogThread/
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/util/udpSocket/
LOCAL_C_INCLUDES += $(SAMPLES_INCL)
LOCAL_C_INCLUDES += $(PUBLIC_INCL)
LOCAL_C_INCLUDES += $(ROOT_PATH)/include/common/
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := cryptoppRsa
LOCAL_SRC_FILES := ../../../../../../3rd_party/rsa_android/obj/local/armeabi/libcryptoppRsa.a
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../../../../../3rd_party/cryptopp/cryptopp-5.6.1
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkApplicationLibrary
LOCAL_SRC_FILES := ../../../../../../iviLink/include_android/native/libiviLinkApplicationLibrary.a
include $(PREBUILT_STATIC_LIBRARY)
