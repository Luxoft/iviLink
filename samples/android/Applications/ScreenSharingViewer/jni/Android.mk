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
LINUX_PATHS         := $(shell find $(LOCAL_PATH)/../../../../linux/ -type d)
ROOT_PATH           := $(LOCAL_PATH)/../../../../..
LINUX_DIR           := $(ROOT_PATH)/samples/linux
UTILS_INCL          := $(shell find $(ROOT_PATH)/framework/utils/ -type d)
APP_LIB             := $(shell find $(ROOT_PATH)/iviLink/include/ -type d)
JSON_PATH           := $(shell find $(ROOT_PATH)/3rd_party/json/ -type d)

#profile impl
include $(CLEAR_VARS)
LOCAL_MODULE := ScreenSharingProfile
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES := $(LINUX_DIR)/profiles/ScreenSharingProfileImpl/CScreenSharingProfileImpl.cpp
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(APP_LIB)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

#appwrapper
include $(CLEAR_VARS)
LOCAL_MODULE := appwrapper
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES += src/com_luxoft_ivilink_samples_screensharingviewer_BridgeC.cpp
LOCAL_SRC_FILES += src/CScreenSharingViewer.cpp
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(APP_LIB)
LOCAL_C_INCLUDES += $(JSON_PATH)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/../../Android.mk # for exported prebuilts
