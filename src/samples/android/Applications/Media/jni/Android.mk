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
LINUX_PATHS := $(shell find $(LOCAL_PATH)/../../../../linux/ -type d)
SRC_PREFIX := $(LOCAL_PATH)/../../../../../../src
UTILS_INCL 	:= $(shell find $(SRC_PREFIX)/utils/ -type d)
FRAMEWORK_INCL 	:= $(shell find $(SRC_PREFIX)/framework/ -type d)

#app
include $(CLEAR_VARS)
LOCAL_MODULE := application
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRC_PREFIX)/samples/linux/MediaApp/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRC_PREFIX)/framework/libraries/trust_list/private/get_mac.cpp)
LOCAL_SRC_FILES += $(wildcard $(SRC_PREFIX)/samples/linux/MediaApp/AndroidImplementations/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi -g
include $(BUILD_SHARED_LIBRARY)

#profile impl
include $(CLEAR_VARS)
LOCAL_MODULE := MediaStreamingServerProfile
LOCAL_STATIC_LIBRARIES := SenderThread iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/MediaStreamingServerProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

#profile impl
include $(CLEAR_VARS)
LOCAL_MODULE := MediaStreamingClientProfile
LOCAL_STATIC_LIBRARIES := SenderThread iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/MediaStreamingClientProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

#profile impl
include $(CLEAR_VARS)
LOCAL_MODULE := MediaControlServerProfile
LOCAL_STATIC_LIBRARIES := SenderThread iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/MediaControlServerProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

#profile impl
include $(CLEAR_VARS)
LOCAL_MODULE := MediaControlClientProfile
LOCAL_STATIC_LIBRARIES := SenderThread iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/MediaControlClientProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

#profile impl
include $(CLEAR_VARS)
LOCAL_MODULE := MediaSourceServerProfile
LOCAL_STATIC_LIBRARIES := SenderThread iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/MediaSourceServerProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

#profile impl
include $(CLEAR_VARS)
LOCAL_MODULE := MediaSourceClientProfile
LOCAL_STATIC_LIBRARIES := SenderThread iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/MediaSourceClientProfile/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SenderThread
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../linux/Profiles/mediaCommon/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_STATIC_LIBRARY)

include $(LOCAL_PATH)/../../Android.mk # for exported prebuilts
