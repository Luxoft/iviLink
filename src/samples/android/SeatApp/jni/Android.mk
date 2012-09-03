LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := seatclient
LOCAL_STATIC_LIBRARIES := appLib iviCommon  protoGenerated
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../linux/Seat_App/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../linux/Seat_App
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SeatSenderProfile
LOCAL_STATIC_LIBRARIES := profileLib iviCommon protoGenerated
LOCAL_SRC_FILES := ../../../linux/Profiles/SeatSenderProfile/CSeatSenderProfile.cpp
LOCAL_SRC_FILES += ../../../linux/Profiles/SeatSenderProfile/CSenderThread.cpp
LOCAL_SRC_FILES += ../../../linux/Profiles/SeatSenderProfile/CProfileFactory.cpp
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SeatReceiverProfile
LOCAL_STATIC_LIBRARIES := profileLib iviCommon protoGenerated
LOCAL_SRC_FILES := ../../../linux/Profiles/SeatReceiverProfile/CSeatReceiverProfile.cpp
LOCAL_SRC_FILES += ../../../linux/Profiles/SeatReceiverProfile/CProfileFactory.cpp
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := protoGenerated
LOCAL_STATIC_LIBRARIES := protobuf
LOCAL_CPP_EXTENSION := .cc
LOCAL_SRC_FILES += ../../../linux/Profiles/ProfileAPI/SeatState/generated/seat-state.pb.cc
LOCAL_SRC_FILES += ../../../linux/Profiles/ProfileAPI/SeatState/generated/timestamp.pb.cc
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../linux/Profiles/ProfileAPI/SeatState/generated/
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_STATIC_LIBRARY)

include $(LOCAL_PATH)/../../Android.mk
