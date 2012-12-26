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

#profile impl
include $(CLEAR_VARS)
LOCAL_MODULE := ScreenSharingProfile
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES := ../../../../linux/Profiles/ScreenSharingProfileImpl/CScreenSharingProfileImpl.cpp
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

#appwrapper
include $(CLEAR_VARS)
LOCAL_MODULE := appwrapper
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES += src/com_luxoft_ivilink_samples_screensharingviewer_BridgeC.cpp
LOCAL_SRC_FILES += src/CScreenSharingViewer.cpp
LOCAL_C_INCLUDES += $(LINUX_PATHS)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(FRAMEWORK_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG  -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/../../Android.mk # for exported prebuilts
