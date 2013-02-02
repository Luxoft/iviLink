export ROOT_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
$(info ROOT_DIR $(ROOT_DIR))

COMMON_MK := $(ROOT_DIR)common.mk
include $(COMMON_MK)
export COMMON_MK

export 3RD_PARTY_SRC_DIR := $(ROOT_DIR)3rd_party/
export 3RD_PARTY_RES_DIR := $(3RD_PARTY_SRC_DIR)result/
export IVILINK_DIR := $(ROOT_DIR)iviLink/
export IVILINK_INCLUDE_DIR := $(IVILINK_DIR)include/
export IVILINK_INCLUDE_COMMON_DIR := $(IVILINK_INCLUDE_DIR)common/
export IVILINK_INCLUDE_APP_LIB_DIR := $(IVILINK_INCLUDE_DIR)ApplicationLibrary/
export IVILINK_INCLUDE_PROFILE_LIB_DIR := $(IVILINK_INCLUDE_DIR)ProfileLibrary/
export IVILINK_INCLUDE_UTILS_DIR := $(IVILINK_INCLUDE_DIR)utils/
export IVILINK_FW_DIR := $(ROOT_DIR)framework/

export IVILINK_REPO_DIR := $(IVILINK_DIR)database/
export IVILINK_PROFILE_API_REPO_DIR := $(IVILINK_REPO_DIR)profileAPIs/
export IVILINK_PROFILE_REPO_DIR := $(IVILINK_REPO_DIR)profiles/
export IVILINK_SERVICE_REPO_DIR := $(IVILINK_REPO_DIR)services/
export IVILINK_APP_REPO_DIR := $(IVILINK_REPO_DIR)applications/

export IVILINK_PROFILE_DIR := $(IVILINK_DIR)profiles/
export IVILINK_LIB_DIR := $(IVILINK_DIR)lib/
export IVILINK_BIN_DIR := $(IVILINK_DIR)bin/
export IVILINK_APP_DIR := $(IVILINK_DIR)applications/

export CONFIG_MK_EXPORTED=true
