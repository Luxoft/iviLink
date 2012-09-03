LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := authenticationProfile
LOCAL_STATIC_LIBRARIES := profileLib iviCommon  cryptoppRsa jsonCpp
MY_FILES := $(wildcard $(LOCAL_PATH)/../../../../../../../../src/samples/linux/Profiles/AuthenticationProfileImpl/*.cpp)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(MY_FILES)
MY_FILES := $(wildcard $(LOCAL_PATH)/../../../../../../../../src/framework/libraries/trust_list/private/*.cpp)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(MY_FILES)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS := -DANDROID -DLATRACE -DLADEBUG  -Wno-psabi
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := authenticationApp
LOCAL_STATIC_LIBRARIES := appLib iviCommon
MY_FILES := $(wildcard $(LOCAL_PATH)/*.cpp)
MY_FILES += $(wildcard $(LOCAL_PATH)/../../../../../../../../src/samples/linux/AuthenticationApplication/*.cpp)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(MY_FILES)
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

# to get profile lib and app lib
include $(LOCAL_PATH)/../../../../../../../../src/samples/android/Android.mk
