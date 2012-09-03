LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := profileLib
LOCAL_SRC_FILES := ../../../src/framework/libraries/AndroidPrebuilt/obj/local/armeabi/libprofileLib.a
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../../src
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../../src/framework/public/profileLib
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := appLib
LOCAL_SRC_FILES := ../../../src/framework/libraries/AndroidPrebuilt/obj/local/armeabi/libappLib.a
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../../src
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../../src/framework/libraries/AppMan/App
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := iviCommon
LOCAL_SRC_FILES := ../../../src/framework/libraries/AndroidPrebuilt/obj/local/armeabi/libiviLinkCommon.a
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../..
LOCAL_EXPORT_C_INCLUDES += $(shell find $(LOCAL_PATH)/../../framework -path $(LOCAL_PATH)/../../framework/*obj* -prune  -o -path $(LOCAL_PATH)/../../framework/*res* -prune -o -path $(LOCAL_PATH)/../../framework/*bin* -prune  -o -path $(LOCAL_PATH)/../../framework/*processEntryPoint* -prune  -o -type d) 
LOCAL_EXPORT_C_INCLUDES += $(shell find $(LOCAL_PATH)/../../utils -path $(LOCAL_PATH)/../../utils/*json* -prune  -o  -type d)
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := protobuf
LOCAL_SRC_FILES := ../../../src/framework/libraries/AndroidPrebuilt/protobuf/libprotobufLiteStatic.a
include $(PREBUILT_STATIC_LIBRARY)

