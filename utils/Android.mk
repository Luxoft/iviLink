LOCAL_PATH := $(call my-dir)

###### LIB Module ######

include $(CLEAR_VARS)
LOCAL_MODULE := LIB

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/.. \
                    $(LOCAL_PATH)/configurator \
                    $(LOCAL_PATH)/ipc \
                    $(LOCAL_PATH)/ipc/helpers \
                    $(LOCAL_PATH)/json/include \
                    $(LOCAL_PATH)/misc \
                    $(LOCAL_PATH)/serialize \
                    $(LOCAL_PATH)/threads \
                    $(LOCAL_PATH)/xml

LOCAL_CPPFLAGS      += -fexceptions
LOCAL_CFLAGS        := -pthread
LOCAL_EXPORT_CFLAGS := -pthread

LOCAL_SRC_FILES := ./misc/private/byteOrder.cpp \
                   ./misc/private/IVILinkLogger.cpp \
                   ./misc/private/CUid.cpp \
                   ./misc/private/CUnixSocket.cpp \
                   ./misc/private/logging.cpp \
                   \
                   ./threads/private/CThreadPool.cpp \
                   ./threads/private/CTimeoutManager.cpp \
                   ./threads/private/CThread.cpp \
                   ./threads/private/CSignalSemaphore.cpp \
                   ./threads/private/CSignalSemaphoreInterproc.cpp \
                   ./threads/private/CMutex.cpp \
                   ./threads/private/CThreadPoolJob.cpp \
                   ./threads/private/CCondVar.cpp \
                   \
                   ./json/src/lib_json/json_reader.cpp \
                   ./json/src/lib_json/json_value.cpp \
                   ./json/src/lib_json/json_writer.cpp \
                   \
                   ./serialize/private/Serialize.cpp \
                   \
                   ./ipc/helpers/private/buffer_helpers.cpp \
                   ./ipc/helpers/private/CBufferManager.cpp \
                   ./ipc/private/CIpc.cpp \
                   ./ipc/private/CIpcSocket.cpp \
                   \
                   ./configurator/private/configurator.cpp \
                   \
                   ./xml/private/pugixml.cpp

#LOCAL_SRC_FILES += ./threads/private/CRWMutex.cpp

LOCAL_LDLIBS := -llog

include $(BUILD_STATIC_LIBRARY)
