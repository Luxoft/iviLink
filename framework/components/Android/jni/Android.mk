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

LOCAL_PATH          := $(call my-dir)

ROOT_PATH           := $(LOCAL_PATH)/../../../..
FW_PREFIX           := $(ROOT_PATH)/framework
MSG_PROTO_PREFIX    := $(ROOT_PATH)/framework/messageProtocols/

UTILS_INCL          := $(shell find $(ROOT_PATH)/framework/utils/ -type d)
UTILS_INCL			+= $(LOCAL_PATH)/../../SystemController/componentLauncher
COMMON_INCL         := $(shell find $(ROOT_PATH)/iviLink/include/ -type d)

CFLAGS              := -DANDROID -DPUGIXML_NO_EXCEPTIONS -Wno-psabi



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
LOCAL_STATIC_LIBRARIES := UdpSocket iviLinkApplicationLibrary
LOCAL_SRC_FILES := src/com_luxoft_ivilink_sdk_CConnectivityAgentWrapper.cpp
LOCAL_SRC_FILES += src/GlobalImplementations.cpp
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/ConnectivityAgent/generic/HAL/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/ConnectivityAgent/generic/L0/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/ConnectivityAgent/generic/L1/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/ConnectivityAgent/generic/ports/android/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/ConnectivityAgent/generic/ports/common/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/SystemController_ConnectivityAgent/ConnectivityAgent/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)/WatchdogThread/
LOCAL_C_INCLUDES += $(FW_PREFIX)/utils/udpSocket/
LOCAL_C_INCLUDES += $(shell find $(FW_PREFIX)/components/ConnectivityAgent/process/ -type d)
LOCAL_C_INCLUDES += $(shell find $(FW_PREFIX)/components/ConnectivityAgent/generic/ -type d)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/SystemController_ConnectivityAgent/ -type d)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)
LOCAL_C_INCLUDES += $(COMMON_INCL)
LOCAL_C_INCLUDES += $(FW_PREFIX)/tests/unit/

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)



#######################################################################################################################################
### Channel Supervisor shared lib #####################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ChannelSupervisor
LOCAL_STATIC_LIBRARIES := UdpSocket iviLinkApplicationLibrary
LOCAL_SRC_FILES := src/com_luxoft_ivilink_sdk_CChannelSupervisorWrapper.cpp
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/SystemController_ChannelSupervisor/ChannelSupervisor/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/ChannelSupervisor/NegotiatorProcess/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/ChannelSupervisor/Map/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/ChannelSupervisor/Watchdog/src/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)/WatchdogThread/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/utils/udpSocket/
LOCAL_C_INCLUDES += $(shell find $(FW_PREFIX)/components/ChannelSupervisor/ -type d)
LOCAL_C_INCLUDES += $(shell find $(FW_PREFIX)/components/ConnectivityAgent/generic/common/ -type d)
LOCAL_C_INCLUDES += $(shell find $(FW_PREFIX)/components/ConnectivityAgent/process/ -type d)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/SystemController_ChannelSupervisor/ -type d)
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)
LOCAL_C_INCLUDES += $(COMMON_INCL)
LOCAL_C_INCLUDES += $(shell find $(ROOT_PATH)/3rd_party/xml/ -type d)

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)



#######################################################################################################################################
### Profile Layer shared lib ##########################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ProfileLayer
LOCAL_STATIC_LIBRARIES := UdpSocket iviLinkApplicationLibrary
LOCAL_SRC_FILES += src/com_luxoft_ivilink_sdk_CProfileManagerWrapper.cpp
LOCAL_SRC_FILES += $(shell find $(FW_PREFIX)/components/ProfileManager/ -name *.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/SystemController_ProfileManager/ProfileManager/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/ProfileManager_ApplicationLibrary/ProfileManager/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/ApplicationManager_ProfileManager/ProfileManager/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)/WatchdogThread/
LOCAL_C_INCLUDES += $(FW_PREFIX)/utils/udpSocket/
LOCAL_C_INCLUDES += $(shell find $(FW_PREFIX)/components/ProfileManager/ -type d)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/SystemController_ProfileManager/ -type d)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/ApplicationManager_ProfileManager/ -type d)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/ProfileManager_ApplicationLibrary/ -type d)
LOCAL_C_INCLUDES += $(FW_PREFIX)/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(FW_PREFIX)/components/ConnectivityAgent/process/
LOCAL_C_INCLUDES += $(FW_PREFIX)/components/ChannelSupervisor/NegotiatorClient/
LOCAL_C_INCLUDES += $(FW_PREFIX)/components/ChannelSupervisor/Tube/
LOCAL_C_INCLUDES += $(FW_PREFIX)/components/ChannelSupervisor/common/
LOCAL_C_INCLUDES += $(FW_PREFIX)/components/ChannelSupervisor/common/Messages/
LOCAL_C_INCLUDES += $(FW_PREFIX)/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)
LOCAL_C_INCLUDES += $(COMMON_INCL)
LOCAL_C_INCLUDES += $(shell find $(ROOT_PATH)/3rd_party/xml/ -type d)


LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)

#######################################################################################################################################
### Application Manager shared lib ####################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ApplicationManager
LOCAL_STATIC_LIBRARIES := UdpSocket iviLinkApplicationLibrary
LOCAL_SRC_FILES += src/com_luxoft_ivilink_sdk_ApplicationManagerWrapper.cpp
LOCAL_SRC_FILES += $(shell find $(FW_PREFIX)/components/ApplicationManager/*/private/*.cpp)
LOCAL_SRC_FILES += $(FW_PREFIX)/components/ApplicationManager/process/SystemControllerMsg.cpp
LOCAL_SRC_FILES += $(shell find $(FW_PREFIX)/components/ApplicationManager/ports/android/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/SystemController_ApplicationManager/ApplicationManager/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/ApplicationManager_ProfileManager/ApplicationManager/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/ApplicationManager_Application/ApplicationManager/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)/WatchdogThread/
LOCAL_C_INCLUDES += $(shell find $(FW_PREFIX)/components/ApplicationManager/ -type d)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/SystemController_ApplicationManager/ -type d)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/ApplicationManager_ProfileManager/ -type d)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX)/ApplicationManager_Application/ -type d)
LOCAL_C_INCLUDES += $(shell find $(ROOT_PATH)/3rd_party/xml/ -type d)
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(COMMON_INCL)


LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)


#######################################################################################################################################
### System Controller shared lib ######################################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := SystemController
LOCAL_STATIC_LIBRARIES := UdpSocket TcpSocket iviLinkApplicationLibrary
LOCAL_SRC_FILES += src/com_luxoft_ivilink_sdk_CWrapper.cpp
LOCAL_SRC_FILES += src/MakeRequest.cpp
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/SystemController/ssm/*/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/*/SystemController/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/*/SystemController/src/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(FW_PREFIX)/libraries/TriggerQueue/
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)/WatchdogThread/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/utils/udpSocket/
LOCAL_C_INCLUDES += $(SRCPREFIX)/framework/libraries/TcpSocket/
LOCAL_C_INCLUDES += $(shell find $(FW_PREFIX)/components/SystemController/ -type d)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX) -type d)
LOCAL_C_INCLUDES += $(COMMON_INCL)


LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)


#######################################################################################################################################
### System Controller Watchdog shared lib #############################################################################################
#######################################################################################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := SystemControllerWatchdog
LOCAL_STATIC_LIBRARIES := UdpSocket TcpSocket iviLinkApplicationLibrary
LOCAL_SRC_FILES += src/com_luxoft_ivilink_sdk_SystemControllerWatchdog.cpp
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/SystemController/watchdog/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/components/SystemController/watchdog/ports/android/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(MSG_PROTO_PREFIX)/*/SystemControllerWatchdog/src/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)/WatchdogThread/
LOCAL_C_INCLUDES += $(FW_PREFIX)/utils/udpSocket/
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)/SystemController_SystemControllerWatchdog/SystemControllerWatchdog/
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)/SystemController_SystemControllerWatchdog/
LOCAL_C_INCLUDES += $(MSG_PROTO_PREFIX)/
LOCAL_C_INCLUDES += $(shell find $(FW_PREFIX)/components/SystemController/ -type d)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(shell find $(MSG_PROTO_PREFIX) -type d)
LOCAL_C_INCLUDES += $(COMMON_INCL)


LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_SHARED_LIBRARY)


#######################################################################################################################################
#######################################################################################################################################
#######################################################################################################################################
### Static libraries ##################################################################################################################
#######################################################################################################################################
#######################################################################################################################################
#######################################################################################################################################

#######################################################################################################################################
### Tcp Socket static lib #############################################################################################################
#######################################################################################################################################

include $(CLEAR_VARS)
LOCAL_MODULE := TcpSocket
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/libraries/TcpSocket/src/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(FW_PREFIX)/libraries/TcpSocket
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(COMMON_INCL)

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_STATIC_LIBRARY)

#######################################################################################################################################
### UDP Socket static lib #############################################################################################################
#######################################################################################################################################

include $(CLEAR_VARS)
LOCAL_MODULE := UdpSocket
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(FW_PREFIX)/utils/udpSocket/src/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(FW_PREFIX)/utils/udpSocket
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(COMMON_INCL)

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += $(CFLAGS)
include $(BUILD_STATIC_LIBRARY)

include $(LOCAL_PATH)/src/authentication/Android.mk
