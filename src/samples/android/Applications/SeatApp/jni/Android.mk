
LOCAL_PATH := $(call my-dir)
LINUX_PATHS := $(shell find $(LOCAL_PATH)/../../../../linux/ -type d)
SRC_PREFIX := $(LOCAL_PATH)/../../../../../../src
UTILS_INCL 	:= $(shell find $(SRC_PREFIX)/utils/ -type d)
FRAMEWORK_INCL 	:= $(shell find $(SRC_PREFIX)/framework/ -type d)


include $(CLEAR_VARS)
LOCAL_MODULE := seatclient
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary SeatState
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Seat_App/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SeatSenderProfile
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary SeatState
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/SeatSenderProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SeatReceiverProfile
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary SeatState
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/SeatReceiverProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := SeatState
LOCAL_STATIC_LIBRARIES := json
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/ProfileAPI/States/Seat/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_STATIC_LIBRARY)

include $(LOCAL_PATH)/../../Android.mk
