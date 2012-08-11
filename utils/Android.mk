LOCAL_PATH := $(call my-dir)

###### LIB Module ######

include $(CLEAR_VARS)
LOCAL_MODULE := LIB


LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/THREADS/include
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/THREADS/include $(LOCAL_PATH)/..

LOCAL_CFLAGS        := -pthread
LOCAL_EXPORT_CFLAGS := -pthread

LOCAL_SRC_FILES := CUnixSocket.cpp
LOCAL_SRC_FILES += byteOrder.cpp
LOCAL_SRC_FILES += logging.cpp
LOCAL_SRC_FILES += THREADS/sources/CCondVar.cpp
LOCAL_SRC_FILES += THREADS/sources/CMutex.cpp
#LOCAL_SRC_FILES += THREADS/sources/CRWMutex.cpp
LOCAL_SRC_FILES += THREADS/sources/CSignalSemaphore.cpp
LOCAL_SRC_FILES += THREADS/sources/CThread.cpp
LOCAL_SRC_FILES += THREADS/sources/CTimeoutManager.cpp


LOCAL_LDLIBS := -llog

include $(BUILD_STATIC_LIBRARY)