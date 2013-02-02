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

ROOT_PATH 	:= $(LOCAL_PATH)/../../../../..
UTILS_INCL 	:= $(shell find $(ROOT_PATH)/framework/utils/ -type d)
APPLIB_INCL := $(shell find $(ROOT_PATH)/iviLink/include/ -type d)
APPLIB_LIB_INCL := $(shell find $(ROOT_PATH)/framework/libraries/ApplicationLibrary/ -type d)
APPLIB_LIB_INCL += $(shell find $(ROOT_PATH)/framework/libraries/ProfileLibrary/ -type d)

#lib
include $(CLEAR_VARS)
LOCAL_MODULE := AndroidCommonLibNative
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/src/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(APPLIB_INCL)
LOCAL_C_INCLUDES += $(APPLIB_LIB_INCL)
LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/components/ConnectivityAgent/generic/common/
LOCAL_C_INCLUDES += $(ROOT_PATH)/framework/libraries/ApplicationLibrary/ServiceManager/

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkApplicationLibrary
LOCAL_SRC_FILES := ../../../native/libiviLinkApplicationLibrary.a
include $(PREBUILT_STATIC_LIBRARY)

