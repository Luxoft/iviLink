LOCAL_PATH          := $(call my-dir)
IVILINK_PREFIX      := $(LOCAL_PATH)/../../../../iviLink
FW_PREFIX           := $(LOCAL_PATH)/../../../../framework
LIB_PREFIX          := $(LOCAL_PATH)/../..
3RD_PARTY_PREFIX    := $(LOCAL_PATH)/../../../../3rd_party

UTILS_INCL          += $(shell find $(FW_PREFIX)/utils/ -type d)
3RD_PARTY_INCL      += $(shell find $(3RD_PARTY_PREFIX)/json/ -type d)
3RD_PARTY_INCL      += $(shell find $(3RD_PARTY_PREFIX)/xml/ -type d)
3RD_PARTY_INCL      += $(3RD_PARTY_PREFIX)
FRAMEWORK_INCL  	+= $(shell find $(IVILINK_PREFIX)/include/ -type d)
FRAMEWORK_INCL      += $(shell find $(LIB_PREFIX)/ApplicationLibrary/ -type d)
FRAMEWORK_INCL      += $(shell find $(LIB_PREFIX)/ProfileLibrary/ -type d)
FRAMEWORK_INCL      += $(shell find $(LIB_PREFIX)/trust_list/ -type d)
FRAMEWORK_INCL      += $(shell find $(FW_PREFIX)/components/ChannelSupervisor/ -type d)
FRAMEWORK_INCL      += $(shell find $(FW_PREFIX)/components/ConnectivityAgent/ -type d)
FRAMEWORK_INCL      += $(shell find $(FW_PREFIX)/messageProtocols/ApplicationManager_Application/ -type d)
FRAMEWORK_INCL      += $(shell find $(FW_PREFIX)/messageProtocols/ProfileManager_ApplicationLibrary/ -type d)
FRAMEWORK_INCL  	+= $(shell find $(IVILINK_PREFIX)/include_android/java/NonNativeSupport/jni/ -type d)


SOURCES             += $(shell find $(LIB_PREFIX)/ -name *.cpp)
SOURCES             += $(shell find $(FW_PREFIX)/utils/ -name *.cpp)
SOURCES             += $(shell find $(3RD_PARTY_PREFIX)/json/ -name *.cpp)
SOURCES             += $(shell find $(3RD_PARTY_PREFIX)/xml/ -name *.cpp)
SOURCES             += $(wildcard $(FW_PREFIX)/messageProtocols/*/Application/private/*.cpp)
SOURCES             += $(wildcard $(FW_PREFIX)/messageProtocols/*/ApplicationLibrary/private/*.cpp)
SOURCES             += $(wildcard $(FW_PREFIX)/components/ChannelSupervisor/Tube/private/*.cpp)
SOURCES             += $(wildcard $(FW_PREFIX)/components/ChannelSupervisor/NegotiatorClient/private/*.cpp)
SOURCES             += $(wildcard $(FW_PREFIX)/components/ChannelSupervisor/common/Messages/*.cpp)
SOURCES             += $(wildcard $(FW_PREFIX)/components/ConnectivityAgent/generic/common/private/*.cpp)    

include $(CLEAR_VARS)
LOCAL_MODULE 		:= iviLinkApplicationLibrary
LOCAL_SRC_FILES 	:= $(SOURCES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES 	+= $(LOCAL_PATH)
LOCAL_C_INCLUDES 	+= $(LOCAL_PATH)/src
LOCAL_C_INCLUDES 	+= $(UTILS_INCL)
LOCAL_C_INCLUDES 	+= $(FRAMEWORK_INCL)
LOCAL_C_INCLUDES    += $(3RD_PARTY_INCL)

LOCAL_LDLIBS 		+= -llog
LOCAL_CFLAGS 		:= -DANDROID -Wno-psabi
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE        := iviLinkApplicationLibraryShared
LOCAL_SRC_FILES     := $(SOURCES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES    += $(LOCAL_PATH)
LOCAL_C_INCLUDES    += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES    += $(UTILS_INCL)
LOCAL_C_INCLUDES    += $(FRAMEWORK_INCL)
LOCAL_C_INCLUDES    += $(3RD_PARTY_INCL)

LOCAL_LDLIBS        += -llog
LOCAL_CFLAGS        := -DANDROID -Wno-psabi
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkApplicationLibraryForceStaticBuild
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
include $(BUILD_SHARED_LIBRARY)

