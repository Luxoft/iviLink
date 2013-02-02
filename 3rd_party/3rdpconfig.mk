#export FW_DIR := $(ROOT_DIR)/framework/
#export RESULT_DIR := $(abspath $(ROOT_DIR)../src_products)
#export PREFIX := $(abspath $(ROOT_DIR)../src_install)
#export MISC_DIR := $(ROOT_DIR)framework/utils/misc/
#export THREADS_DIR := $(ROOT_DIR)framework/utils/threads/
#export UNITTEST_DIR := $(ROOT_DIR)tests/unit/
#
#export SYSTEM_CONTROLLER_PROCESS_NAME := IVILinkSystemController
#export CONNECTIVITY_AGENT_PROCESS_NAME := IVILinkConnectivityAgent
#export APPLICATION_MANAGER_PROCESS_NAME := IVILinkApplicationManager
#export NEGOTIATOR_PROCESS_NAME := IVILinkNegotiator
#export PROFILE_MANAGER_PROCESS_NAME := IVILinkProfileManager
#export PROFILE_REPOSITORY_PROCESS_NAME := IVILinkProfileRepository
#export CONNECTIVITY_AGENT_LIBRARY_NAME := libConnectivityAgent.so
#export SYSTEM_CONTROLLER_WATCHDOG_PROCESS_NAME := IVILinkSystemControllerWatchdog
#export AUTHENTICATION_APPLICATION_PROCESS_NAME := AuthenticationApplication
#export DEVICE_MANAGER_PROCESS_NAME := DeviceManager
#export PROFILE_REPOSITORY := $(PREFIX)/profile_repository
#export SERVICE_REPOSITORY := $(PREFIX)/service_repository
#export APPLICATION_MANAGER_REPOSITORY := $(PREFIX)/application_manager_repository
#export PROGRESSBAR_PROCESS_NAME := IVILinkProgressBar
export 3RDPDEBUG_FLAGS := -O0 -ggdb
export 3RDPRELEASE_FLAGS := -O2 -D NDEBUG

3RDPGLOBAL_CFLAGS   := -I$(ROOT_DIR) -Wall -pthread -fPIC -I$(3RD_PARTY_RES_DIR)include -I$(MISC_DIR) -I$(THREADS_DIR) #-Werror


3RDPGLOBAL_LDFLAGS  := -L$(ROOT_DIR) -lrt -pthread -L$(3RD_PARTY_RES_DIR)lib -Wl,-rpath,$(PREFIX)

export  3RDPUSELOG4CPLUS:=true
export  3RDPUSELOG4CPLUS_DYNAMIC:=true
ifeq ($(3RDPUSELOG4CPLUS),true)
3RDPGLOBAL_CFLAGS += -DUSELOG4CPLUS
ifeq ($(3RDPUSELOG4CPLUS_DYNAMIC),true)
3RDPGLOBAL_LDFLAGS += -llog4cplus -Wl,-rpath,$(3RD_PARTY_RES_DIR)lib
else
3RDPGLOBAL_LDFLAGS += -Wl,-Bstatic -llog4cplus -Wl,-Bdynamic
endif # 3RDPUSELOG4CPLUS_DYNAMIC
endif # 3RDPUSELOG4CPLUS


3RDPGLOBAL_CXXFLAGS := $(3RDPGLOBAL_CFLAGS)

GLOBAL_DLL_CFLAGS   := 
GLOBAL_DLL_CXXFLAGS := $(3RDPGLOBAL_DLL_CFLAGS)
GLOBAL_DLL_LDFALGS  :=  


export 3RDPGLOBAL_CFLAGS
export 3RDPGLOBAL_CXXFLAGS
export 3RDPGLOBAL_LDFLAGS
export 3RDPGLOBAL_DLL_CFLAGS
export 3RDPGLOBAL_DLL_CXXFLAGS
export 3RDPGLOBAL_DLL_LDFALGS

export NOTHING:=
export SPACE:=$(NOTHING) $(NOTHING)
export COMMA:=,

export 3RDPCONFIG_MK_EXPORTED=true
