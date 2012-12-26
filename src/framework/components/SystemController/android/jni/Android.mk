#  iviLINK SDK, version 0.9 (for preview only)                                      
#    http://www.ivilink.net                                                         
#  Cross Platform Application Communication Stack for In-Vehicle Applications       
#                                                                                   
#  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
#                                                                                   
#  This library is free software; you can redistribute it and/or                    
#  modify it under the terms of the GNU Lesser General Public                       
#  License as published by the Free Software Foundation; version 2.1.               
#                                                                                   
#  This library is distributed in the hope that it will be useful,                  
#  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
#  Lesser General Public License for more details.                                  
#                                                                                   
#  You should have received a copy of the GNU Lesser General Public                 
#  License along with this library; if not, write to the Free Software              
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
# 
# 
# 
# 

LOCAL_PATH := $(call my-dir)
SRCPREFIX := $(LOCAL_PATH)/../../../../../../src
MSG_PROTO_PREFIX := $(SRCPREFIX)/framework/messageProtocol/
UTILS_INCL := $(shell find $(SRCPREFIX)/utils/ -type d)
CFLAGS := -DANDROID -DLATRACE -DLADEBUG -DPUGIXML_NO_EXCEPTIONS -Wno-psabi



#######################################################################################################################################
#######################################################################################################################################
#######################################################################################################################################
### Shared libraries ##################################################################################################################
#######################################################################################################################################
#######################################################################################################################################
#######################################################################################################################################



#######################################################################################################################################
### Connectivity Agent  shared lib ####################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ConnectivityAgent
LOCAL_STATIC_LIBRARIES := ConnectivityAgentCommon Utils 
LOCAL_SRC_FILES := src/com_luxoft_ivilink_sdk_CConnectivityAgentWrapper.cpp
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/HAL/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/L0/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/L1/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/messageProtocol/SystemController_ConnectivityAgent/ConnectivityAgent/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/ConnectivityAgent/process/ -type d)
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/ -type d)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/SystemController_ConnectivityAgent/ -type d)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)



#######################################################################################################################################
### Channel Supervisor shared lib #####################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ChannelSupervisor
LOCAL_STATIC_LIBRARIES := ChannelSupervisorCommon ConnectivityAgentCommon Utils 
LOCAL_SRC_FILES := src/com_luxoft_ivilink_sdk_CChannelSupervisorWrapper.cpp
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/messageProtocol/SystemController_ChannelSupervisor/ChannelSupervisor/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ChannelSupervisor/NegotiatorProcess/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ChannelSupervisor/Map/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/ChannelSupervisor/ -type d)
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/common/ -type d)
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/ConnectivityAgent/process/ -type d)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/SystemController_ChannelSupervisor/ -type d)
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)



#######################################################################################################################################
### Profile Layer shared lib ##########################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ProfileLayer
LOCAL_STATIC_LIBRARIES := ChannelSupervisorCommon ConnectivityAgentCommon Utils 
LOCAL_SRC_FILES += src/com_luxoft_ivilink_sdk_CProfileManagerWrapper.cpp
LOCAL_SRC_FILES += src/com_luxoft_ivilink_sdk_CProfileRepositoryWrapper.cpp
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ProfileManager/PMP/*/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ProfileManager/PMP/process/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ProfileRepository/process/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/libraries/AppMan/Pmp/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ProfileRepository/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/messageProtocol/SystemController_ProfileManager/ProfileManager/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/ProfileManager/PMP -type d)
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/ProfileManager/IpcProtocol -type d)
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/ProfileRepository/ -type d)
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/libraries/AppMan/Pmp/ -type d)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/SystemController_ProfileManager/ -type d)
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ProfileRepository/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ConnectivityAgent/process/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ChannelSupervisor/NegotiatorClient/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ChannelSupervisor/Tube/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ChannelSupervisor/common/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ChannelSupervisor/common/Messages/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)


LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)

#######################################################################################################################################
### Application Manager shared lib ####################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ApplicationManager
LOCAL_STATIC_LIBRARIES := Utils 
LOCAL_SRC_FILES += src/com_luxoft_ivilink_sdk_CApplicationManagerWrapper.cpp
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/AppMan/*/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/AppMan/process/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/libraries/AppMan/AmpForPmp/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/libraries/AppMan/AmpForApp/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/messageProtocol/SystemController_AppMan/AppMan/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/AppMan/ -type d)
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/libraries/AppMan/ -type d)
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/messageProtocol/SystemController_AppMan/ -type d)
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)
LOCAL_C_INCLUDES += $(UTILS_INCL)


LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)


#######################################################################################################################################
### System Controller shared lib ######################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := SystemController
LOCAL_STATIC_LIBRARIES := Utils 
LOCAL_SRC_FILES += src/com_luxoft_ivilink_sdk_CWrapper.cpp
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/SystemController/ssm/*/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/messageProtocol/SystemController_AppMan/SystemController/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/messageProtocol/SystemController_Authentication/SystemController/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/messageProtocol/SystemController_ChannelSupervisor/SystemController/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/messageProtocol/SystemController_ConnectivityAgent/SystemController/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/messageProtocol/SystemController_ProfileManager/SystemController/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(shell find $(SRCPREFIX)/framework/components/SystemController/ -type d)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX) -type d)


LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)


#######################################################################################################################################
#######################################################################################################################################
#######################################################################################################################################
### Helper static libraries ###########################################################################################################
#######################################################################################################################################
#######################################################################################################################################
#######################################################################################################################################



#######################################################################################################################################
### Utils static lib ##################################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := Utils
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/utils/*/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/utils/ipc/helpers/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(UTILS_INCL)

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_STATIC_LIBRARY)



#######################################################################################################################################
### Connectivity Agent common static lib ##############################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ConnectivityAgentCommon
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/common/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ConnectivityAgent/process/
LOCAL_C_INCLUDES += $(UTILS_INCL)

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_STATIC_LIBRARY)


#######################################################################################################################################
### Channel Supervisor common static lib ##############################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ChannelSupervisorCommon
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ChannelSupervisor/Tube/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ChannelSupervisor/NegotiatorClient/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRCPREFIX)/framework/components/ChannelSupervisor/common/Messages/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ChannelSupervisor/NegotiatorClient/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ChannelSupervisor/Tube/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ChannelSupervisor/common/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ChannelSupervisor/common/Messages/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(UTILS_INCL)

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_STATIC_LIBRARY)

include $(LOCAL_PATH)/src/authentication/Android.mk
