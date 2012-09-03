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

ifneq ($(SETUPIVI), true)
include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkCommon
#ssm
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/SystemController/ssm/*/private/*.cpp)
#utils (json excluded)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/utils/*/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/utils/ipc/helpers/private/*.cpp)
#components message protocol
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/messageProtocol/*/*/private/*.cpp)
#appman
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/AppMan/*/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/AppMan/*/private/*.cpp) 
LOCAL_SRC_FILES += ../../../../../src/framework/libraries/AppMan/AmpForApp/private/CAppManProtoServer.cpp
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/libraries/AppMan/AmpForPmp/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/libraries/AppMan/Pmp/private/*.cpp)
#negotiator
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/ChannelSupervisor/*/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/ChannelSupervisor/common/Messages/*.cpp)
#agent
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/ConnectivityAgent/generic/*/private/*.cpp)
#profman
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/ProfileManager/*/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/ProfileManager/PMP/process/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/ProfileManager/*/*/private/*.cpp)
#profrepo
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/ProfileRepository/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/components/ProfileRepository/process/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
#include paths
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../src #path to repositoryroot/src
LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/../../../../framework -path $(LOCAL_PATH)/../../../../framework/*obj* -prune  -o -path $(LOCAL_PATH)/../../../../framework/*res* -prune -o -path $(LOCAL_PATH)/../../../../framework/*bin* -prune  -o -path $(LOCAL_PATH)/../../../../framework/*processEntryPoint* -prune  -o -type d) #excluding android build products paths
LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/../../../../utils -path $(LOCAL_PATH)/../../../../utils/*json* -prune  -o  -type d)

LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -DPUGIXML_NO_EXCEPTIONS -Wno-psabi
LOCAL_LDLIBS := -llog
include $(BUILD_STATIC_LIBRARY)

#trick to force ndk-build actually do smth
include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkCommonShared
LOCAL_STATIC_LIBRARIES := iviLinkCommon
include $(BUILD_SHARED_LIBRARY)
endif

ifeq ($(SETUPIVI), true)
#jsoncpp prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := JsonCpp
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/utils/json/src/lib_json/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../src/utils/json/src/lib_json
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../src/utils/json/include
include $(BUILD_STATIC_LIBRARY)

#trick to force ndk-build actually do smth
include $(CLEAR_VARS)
LOCAL_MODULE := JsonCppShared
LOCAL_STATIC_LIBRARIES := JsonCpp
include $(BUILD_SHARED_LIBRARY)
endif

ifneq ($(SETUPIVI), true)
#profile library - to be linked to every android profile implementation
include $(CLEAR_VARS)
LOCAL_MODULE := profileLib
LOCAL_STATIC_LIBRARIES := iviLinkCommon
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/../../../../../src/framework/public/profileLib/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../src/framework/public/profileLib
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -DPUGIXML_NO_EXCEPTIONS -Wno-psabi
include $(BUILD_STATIC_LIBRARY)

#trick to force ndk-build actually do smth
include $(CLEAR_VARS)
LOCAL_MODULE := profileLibShared
LOCAL_STATIC_LIBRARIES := profileLib
include $(BUILD_SHARED_LIBRARY)

#application library - to be linked to every android application
include $(CLEAR_VARS)
LOCAL_MODULE := appLib
LOCAL_STATIC_LIBRARIES := iviLinkCommon
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/public/appLib/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/libraries/ServiceManager/private/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../../../../src/framework/libraries/AppMan/App/private/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../src/framework/libraries/AppMan/App
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -DPUGIXML_NO_EXCEPTIONS -Wno-psabi
include $(BUILD_STATIC_LIBRARY)

#trick to force ndk-build actually do smth
include $(CLEAR_VARS)
LOCAL_MODULE := appLibShared
LOCAL_STATIC_LIBRARIES := appLib
include $(BUILD_SHARED_LIBRARY)
endif 

ifeq ($(SETUPIVI), true)
#lib for authentication
include $(CLEAR_VARS)
LOCAL_MODULE := cryptoppRsa
SRCPREFIX := ../../../../../src/3rd_party/cryptopp/cryptopp-5.6.1
LOCAL_SRC_FILES += $(SRCPREFIX)/algebra.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/algparam.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/asn.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/basecode.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/cpu.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/cryptlib.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/dll.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/dsa.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/ec2n.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/ecp.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/filters.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/fips140.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/gf2n.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/gfpcrypt.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/hex.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/hmac.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/hrtimer.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/integer.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/iterhash.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/misc.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/modes.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/mqueue.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/nbtheory.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/oaep.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/osrng.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/pkcspad.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/pubkey.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/queue.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/randpool.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/rdtables.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/rijndael.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/rng.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/rsa.cpp
LOCAL_SRC_FILES += $(SRCPREFIX)/sha.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(SRCPREFIX)
include $(BUILD_STATIC_LIBRARY)

#trick to force ndk-build actually do smth
include $(CLEAR_VARS)
LOCAL_MODULE := cryptoppRsaShared
LOCAL_STATIC_LIBRARIES := cryptoppRsa
include $(BUILD_SHARED_LIBRARY)
endif



