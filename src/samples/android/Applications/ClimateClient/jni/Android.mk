
LOCAL_PATH := $(call my-dir)

LINUX_PATHS := $(shell find $(LOCAL_PATH)/../../../../linux/ -type d)

SRC_PREFIX := $(LOCAL_PATH)/../../../../../../src

UTILS_INCL 	:= $(shell find $(SRC_PREFIX)/utils/ -type d)
FRAMEWORK_INCL 	:= $(shell find $(SRC_PREFIX)/framework/ -type d)

include $(CLEAR_VARS)
LOCAL_MODULE := climateapp
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary ClimateState
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Climate_App/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ClimateSenderProfile
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary ClimateState
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/ClimateSenderProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ClimateReceiverProfile
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary ClimateState
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/ClimateReceiverProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := ClimateState
LOCAL_STATIC_LIBRARIES := json
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/ProfileAPI/States/Climate/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_STATIC_LIBRARY)

include $(LOCAL_PATH)/../../Android.mk
