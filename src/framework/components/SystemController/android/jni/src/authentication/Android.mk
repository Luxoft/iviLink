
LOCAL_PATH := $(call my-dir)

SRC_PREFIX 		:= $(LOCAL_PATH)/../../../../../../../../src
UTILS_INCL 		:= $(shell find $(SRC_PREFIX)/utils/ -type d)
SAMPLES_INCL 	:= $(shell find $(SRC_PREFIX)/samples/linux/ -type d)
PMAL_INCL  		:= $(shell find $(SRC_PREFIX)/framework/components/ProfileManager/ -type d)


include $(CLEAR_VARS)
LOCAL_MODULE := authenticationProfile
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary cryptoppRsa jsonCpp
MY_FILES := $(wildcard $(SRC_PREFIX)/samples/linux/Profiles/AuthenticationProfileImpl/*.cpp)
MY_FILES += $(wildcard $(SRC_PREFIX)/framework/libraries/trust_list/private/*.cpp)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(MY_FILES)
$(info auth profile src: $(LOCAL_SRC_FILES))

LOCAL_C_INCLUDES := $(UTILS_INCL)
LOCAL_C_INCLUDES += $(PMAL_INCL)
LOCAL_C_INCLUDES += $(SAMPLES_INCL)
LOCAL_C_INCLUDES += $(shell find $(SRC_PREFIX)/framework/components/ConnectivityAgent/ -type d)
LOCAL_C_INCLUDES += $(shell find $(SRC_PREFIX)/framework/components/ChannelSupervisor/ -type d)
LOCAL_C_INCLUDES += $(shell find $(SRC_PREFIX)/framework/components/ProfileManager/ -type d)
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/public/appLib/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/libraries/trust_list/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/libraries/trust_list/private
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/public/profileLib/

LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG  -Wno-psabi
include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := authenticationApp
LOCAL_STATIC_LIBRARIES := iviLinkApplicationLibrary
MY_FILES := $(wildcard $(LOCAL_PATH)/*.cpp)
MY_FILES += $(wildcard $(SRC_PREFIX)/samples/linux/AuthenticationApplication/*.cpp)
MY_FILES += $(wildcard $(SRC_PREFIX)/framework/messageProtocol/SystemController_Authentication/Authentication/private/*.cpp)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(MY_FILES)
$(info auth app src: $(LOCAL_SRC_FILES))

LOCAL_C_INCLUDES += $(UTILS_INCL)
LOCAL_C_INCLUDES += $(PMAL_INCL)
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/messageProtocol/SystemController_Authentication/Authentication/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/messageProtocol/SystemController_Authentication/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/messageProtocol/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/public/appLib/
LOCAL_C_INCLUDES += $(SRC_PREFIX)/framework/public/profileLib/
LOCAL_C_INCLUDES += $(SAMPLES_INCL)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := cryptoppRsa
LOCAL_SRC_FILES := ../../../../../../../../src/framework/libraries/AndroidPrebuilt/obj/local/armeabi/libcryptoppRsa.a
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../../../../../../../src/3rd_party/cryptopp/cryptopp-5.6.1
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := jsonCpp
LOCAL_SRC_FILES := ../../../../../../../../src/framework/libraries/AndroidPrebuilt/obj/local/armeabi/libJsonCpp.a
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../../../../../../../src/3rd_party/cryptopp/cryptopp-5.6.1
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := iviLinkApplicationLibrary
LOCAL_SRC_FILES := ../../../../../../../../src/framework/libraries/AndroidPrebuilt/AndroidNativeAppLib/obj/local/armeabi/libiviLinkApplicationLibrary.a
include $(PREBUILT_STATIC_LIBRARY)
