LOCAL_PATH := $(call my-dir)
SRC_PREFIX := $(LOCAL_PATH)/../../../../../../src
UTILS_INCL 			:= $(shell find $(SRC_PREFIX)/utils/ -type d)
FRAMEWORK_INCL 		:= $(shell find $(SRC_PREFIX)/framework/components/ProfileManager/PMAL -type d)
FRAMEWORK_INCL      += $(shell find $(SRC_PREFIX)/framework/components/ChannelSupervisor/ -type d)
FRAMEWORK_INCL      += $(shell find $(SRC_PREFIX)/framework/components/ConnectivityAgent/generic -type d)
FRAMEWORK_INCL      += $(shell find $(SRC_PREFIX)/framework/components/SystemController/android/jni/src/ -type d)
FRAMEWORK_INCL      += $(shell find $(SRC_PREFIX)/framework/libraries/ServiceManager -type d)
FRAMEWORK_INCL      += $(shell find $(SRC_PREFIX)/framework/libraries/AppMan -type d)
FRAMEWORK_INCL      += $(shell find $(SRC_PREFIX)/framework/public/AndroidCommonLib/jni -type d)
FRAMEWORK_INCL      += $(SRC_PREFIX)/framework/components/ProfileManager/IpcProtocol/
FRAMEWORK_INCL      += $(SRC_PREFIX)/framework/components/ConnectivityAgent/process/
FRAMEWORK_INCL      += $(SRC_PREFIX)/framework/public/appLib
FRAMEWORK_INCL      += $(SRC_PREFIX)/framework/public/profileLib
FRAMEWORK_INCL      += $(SRC_PREFIX)/framework/public/appLib/private
FRAMEWORK_INCL      += $(SRC_PREFIX)/framework/public/profileLib/private


include $(CLEAR_VARS)
LOCAL_MODULE 		:= iviLinkApplicationLibrary
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/public/profileLib/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/public/appLib/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/utils/*/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/utils/ipc/helpers/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/components/ProfileManager/PMAL/*/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/components/ProfileManager/PMAL/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/components/ChannelSupervisor/Tube/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/components/ChannelSupervisor/NegotiatorClient/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/components/ChannelSupervisor/common/Messages/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/components/ConnectivityAgent/generic/common/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/libraries/ServiceManager/private/*.cpp)
LOCAL_SRC_FILES 	+= $(wildcard $(SRC_PREFIX)/framework/libraries/AppMan/App/private/*.cpp)
LOCAL_SRC_FILES 	:= $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES 	+= $(LOCAL_PATH)
LOCAL_C_INCLUDES 	+= $(LOCAL_PATH)/src
LOCAL_C_INCLUDES 	+= $(UTILS_INCL)
LOCAL_C_INCLUDES 	+= $(FRAMEWORK_INCL)

LOCAL_LDLIBS 		+= -llog
LOCAL_CFLAGS 		:= -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE 			:= iviLinkApplicationLibraryShared
LOCAL_STATIC_LIBRARIES 	:= iviLinkApplicationLibrary
include $(BUILD_SHARED_LIBRARY)
