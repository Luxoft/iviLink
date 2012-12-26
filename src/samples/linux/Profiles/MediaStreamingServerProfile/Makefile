# iviLINK SDK, version 1.1.19
# http://www.ivilink.net
# Cross Platform Application Communication Stack for In-Vehicle Applications
# 
# Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; version 2.1.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
# 

###############################################################################
# Autodetection of config.mk and includes
ifndef CONFIG_MK_EXPORTED
ifdef AXIS_CONFIG_MK
include $(AXIS_CONFIG_MK)
else
$(error config.mk is not exported and AXIS_CONFIG_MK variable cannot be found)
endif
endif
ifndef COMMON_MK
$(error COMMON_MK is not defined)
endif
include $(COMMON_MK)

###############################################################################
# Subfolders
define SUBFOLDERS
endef
define TEST_SUBFOLDERS
endef
###############################################################################
# This target

SRC_DIR := .
SRC := $(wildcard $(SRC_DIR)/*.cpp)

OUT_FNAME     := libMediaStreamingServerProfile.so

CHANNEL_SV_DIR	:= $(ROOT_DIR)framework/components/ChannelSupervisor/
PMAL_DIR	  	:= $(ROOT_DIR)framework/components/ProfileManager/PMAL/
FW_DIR		  	:= $(ROOT_DIR)framework/
PROFILES_DIR  	:= $(ROOT_DIR)samples/linux/Profiles/

INCLUDES      += -I.
INCLUDES      += -I$(CHANNEL_SV_DIR)common/
INCLUDES      += -I$(CHANNEL_SV_DIR)Tube/
INCLUDES      += -I$(PMAL_DIR)PIM/private/
INCLUDES      += -I$(FW_DIR)public/appLib/
INCLUDES      += -I$(FW_DIR)components/ProfileManager/PMAL
INCLUDES      += -I$(FW_DIR)components/ChannelSupervisor/Tube/
INCLUDES      += -I$(FW_DIR)public/profileLib/
INCLUDES      += -I$(PROFILES_DIR)ProfileAPI/
INCLUDES      += -I$(PROFILES_DIR)mediaCommon

CXXFLAGS       = $(GLOBAL_CXXFLAGS) $(GLOBAL_DLL_CXXFLAGS) $(INCLUDES)
CFLAGS         = $(GLOBAL_CFLAGS) $(GLOBAL_DLL_CFLAGS) $(INCLUDES) -std=c99
LDFLAGS       := $(GLOBAL_LDFLAGS) $(GLOBAL_DLL_LDFALGS)

LIBS :=
LIBS += samples/linux/Profiles/mediaCommon/mediaCommon.a
LIBS += utils/threads/libThreads.a

LIBS := $(addprefix $(RESULT_DIR)/,$(LIBS))

###############################################################################
# Variables

OUT_PATH := $(call OUT_PATH_FUNC)
$(info OUT_PATH $(OUT_PATH))

OUT_FULLDIR   := $(OUT_PATH)
OUT_FULLFNAME := $(OUT_FULLDIR)$(OUT_FNAME)

OBJ_FULLDIR := $(OUT_PATH)$(OBJ_DIR)
OBJS        := $(SRC:.cpp=.o)
OBJS        := $(OBJS:.c=.o)
OBJS        := $(addprefix $(OBJ_FULLDIR)/,$(notdir $(OBJS)))
DEPS        := $(OBJS:.o=.d)

###############################################################################
# Rules

$(OUT_FULLDIR):
	mkdir -p $@

$(OBJ_FULLDIR):
	mkdir -p $@

$(OUT_FULLFNAME): $(OBJS) $(LIBS) | $(OUT_FULLDIR)
	$(CXX) $(OBJS) -shared -Wl,--start-group $(LIBS) -Wl,--end-group $(LDFLAGS) -o $@

$(OBJ_FULLDIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_FULLDIR)
	$(CXX) -c $< $(CXXFLAGS) -MMD -o $@

debug: $(OUT_FULLFNAME)
	$(SUBFOLDERS)

release: $(OUT_FULLFNAME)
	$(SUBFOLDERS)

test: $(OUT_FULLFNAME)
	$(SUBFOLDERS)
	$(TEST_SUBFOLDERS)

runtest: $(OUT_FULLFNAME)
	$(SUBFOLDERS)
	$(TEST_SUBFOLDERS)

clean:
	-rm -rf $(ALL_OUT_DIRS)
	$(SUBFOLDERS)
	$(TEST_SUBFOLDERS)

.PHONY: debug release clean test runtest
 

ifeq (,$(findstring $(MAKECMDGOALS),dist clean depend doc docs))
-include $(DEPS)
endif
