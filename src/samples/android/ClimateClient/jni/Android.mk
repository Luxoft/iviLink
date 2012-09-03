LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := climateapp
LOCAL_STATIC_LIBRARIES := appLib iviCommon  protoGenerated
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../linux/Climate_App/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../linux/Climate_App
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ClimateSenderProfile
LOCAL_STATIC_LIBRARIES := profileLib iviCommon protoGenerated
LOCAL_SRC_FILES := ../../../linux/Profiles/ClimateSenderProfile/CClimateSenderProfile.cpp
LOCAL_SRC_FILES += ../../../linux/Profiles/ClimateSenderProfile/CSenderThread.cpp
LOCAL_SRC_FILES += ../../../linux/Profiles/ClimateSenderProfile/CProfileFactory.cpp
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ClimateReceiverProfile
LOCAL_STATIC_LIBRARIES := profileLib iviCommon protoGenerated
LOCAL_SRC_FILES := ../../../linux/Profiles/ClimateReceiverProfile/CClimateReceiverProfile.cpp
LOCAL_SRC_FILES += ../../../linux/Profiles/ClimateReceiverProfile/CProfileFactory.cpp
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := protoGenerated
LOCAL_STATIC_LIBRARIES := protobuf
LOCAL_CPP_EXTENSION := .cc
LOCAL_SRC_FILES += ../../../linux/Profiles/ProfileAPI/ClimateState/generated/climate-state.pb.cc
LOCAL_SRC_FILES += ../../../linux/Profiles/ProfileAPI/ClimateState/generated/timestamp.pb.cc
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../linux/Profiles/ProfileAPI/ClimateState/generated/
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG
include $(BUILD_STATIC_LIBRARY)

include $(LOCAL_PATH)/../../Android.mk
