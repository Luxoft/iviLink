# $Id$

export ROOT_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
$(info ROOT_DIR $(ROOT_DIR))

COMMON_MK := $(ROOT_DIR)common.mk
include $(COMMON_MK)
export COMMON_MK






#export LIB_DIR := lib
export BIN_DIR := bin
export OBJ_DIR := obj

#export RESULT_DIR := .
export RESULT_DIR := $(ROOT_DIR)/lib

GLOBAL_CFLAGS   := -I$(ROOT_DIR) -Wall -pthread -ggdb  -fPIC

#NONUNIQUE_AXIS_QUEUE:=true
ifeq ($(NONUNIQUE_AXIS_QUEUE),true) 
# Set server and client daemons listen on different sockets.
GLOBAL_CFLAGS += -DNONUNIQUE_AXIS_QUEUE
endif

GLOBAL_CXXFLAGS := $(GLOBAL_CFLAGS)

GLOBAL_LDFLAGS  := -L$(ROOT_DIR) -lrt

GLOBAL_DLL_CFLAGS   := 
GLOBAL_DLL_CXXFLAGS := $(GLOBAL_DLL_CFLAGS)
GLOBAL_DLL_LDFALGS  :=  


export GLOBAL_CFLAGS
export GLOBAL_CXXFLAGS
export GLOBAL_LDFLAGS
export GLOBAL_DLL_CFLAGS
export GLOBAL_DLL_CXXFLAGS
export GLOBAL_DLL_LDFALGS


export CONFIG_MK_EXPORTED=true
