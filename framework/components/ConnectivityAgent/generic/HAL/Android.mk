LOCAL_PATH := $(call my-dir)

###### HAL Module ######
include $(CLEAR_VARS)
LOCAL_MODULE := HAL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SRC_FILES := source/CTcpCarrierAdapter.cpp
LOCAL_SRC_FILES += source/CAdapterFactory.cpp
LOCAL_SRC_FILES += source/CCarrierAdapter.cpp
LOCAL_SRC_FILES += source/CConnectionFinder.cpp
LOCAL_SRC_FILES += source/CConnectivityAgent.cpp
LOCAL_SRC_FILES += source/CConnectivityManager.cpp
LOCAL_SRC_FILES += source/CTcpAutoConnectionFinder.cpp
LOCAL_SRC_FILES += source/CDummyConnectionFinder.cpp
LOCAL_SRC_FILES += source/HALInterface.cpp
LOCAL_SRC_FILES += source/CTransmitDispatcher.cpp
LOCAL_SRC_FILES += source/CIncomeDispatcher.cpp
LOCAL_SRC_FILES += source/CTransmittedFramesQueue.cpp
LOCAL_SRC_FILES += source/CQoS.cpp
LOCAL_SRC_FILES += source/CNetlinkSocket.cpp
LOCAL_SRC_FILES += source/CSource.cpp
LOCAL_SRC_FILES += source/CTarget.cpp

LOCAL_STATIC_LIBRARIES := LIB
LOCAL_LDLIBS := -llog

include $(BUILD_STATIC_LIBRARY)
