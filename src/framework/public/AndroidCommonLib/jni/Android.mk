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

LOCAL_PATH 	:= $(call my-dir)

SRC_PREFIX 	:= $(LOCAL_PATH)/../../../../../src
UTILS_INCL 	:= $(shell find $(SRC_PREFIX)/utils/ -type d)
PMAL_INCL 	:= $(shell find $(SRC_PREFIX)/framework/components/ProfileManager/PMAL/ -type d)

#lib
include $(CLEAR_VARS)
LOCAL_MODULE := AndroidCommonLibNative
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/src/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../appLib/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../profileLib/
LOCAL_C_INCLUDES += $(PMAL_INCL)
LOCAL_C_INCLUDES += $(UTILS_INCL)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../appLib/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../profileLib/
LOCAL_C_INCLUDES += $(PMAL_INCL)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/components/ChannelSupervisor/NegotiatorClient/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/components/ChannelSupervisor/Tube/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/components/ChannelSupervisor/common/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/components/ChannelSupervisor/common/Messages/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/components/ConnectivityAgent/process/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/libraries/ServiceManager/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/libraries/ServiceManager/private/

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkApplicationLibrary
LOCAL_SRC_FILES := ../../../libraries/AndroidPrebuilt/AndroidNativeAppLib/obj/local/armeabi/libiviLinkApplicationLibrary.a
include $(PREBUILT_STATIC_LIBRARY)

