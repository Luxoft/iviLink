# iviLINK SDK, version 1.2
# http://www.ivilink.net
# Cross Platform Application Communication Stack for In-Vehicle Applications
# 
# Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
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
###############################################################################
# This target

SRC_DIR := .
SRC := $(wildcard $(SRC_DIR)/*.cpp)

OUT_FNAME     := AppLibTest7

PMAL_DIR	  := $(ROOT_DIR)framework/components/ProfileManager/PMAL/
FW_DIR		  := $(ROOT_DIR)framework/
PROFILES_DIR  := $(ROOT_DIR)samples/linux/Profiles/

INCLUDES      += -I.
INCLUDES	     += -I$(IVILINK_INCLUDE_COMMON_DIR)
INCLUDES	     += -I$(IVILINK_INCLUDE_APP_LIB_DIR)

INCLUDES      += -I../../serviceSources/
INCLUDES      += -I../../AppLibTestUtils/

CXXFLAGS      := $(GLOBAL_CXXFLAGS) $(INCLUDES)
LDFLAGS       := -ldl $(GLOBAL_LDFLAGS)

LIBS :=
LIBS += framework/tests/component/ApplicationLibrary/AppLibTestUtils/AppLibTestUtils.a

DLIBS := 
DLIBS += framework/sharedLibs/applicationSharedLib/libIviLinkApp.so
DLIBS += framework/components/ChannelSupervisor/ChannelLib/libIviLinkChannel.so
DLIBS += framework/utils/sharedLib/libIviLinkUtils.so

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

$(OUT_FULLFNAME): $(OBJS) $(call GET_DEPS, $(LIBS) $(DLIBS)) | $(OUT_FULLDIR) 
	$(CXX) $(OBJS) $(call GET_LIB_LDFLAGS,$(LIBS),$(DLIBS)) $(LDFLAGS) -o $@


$(OBJ_FULLDIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_FULLDIR)
	$(CXX) -c $< $(CXXFLAGS) -MMD -o $@

debug:  $(OUT_FULLFNAME)
	$(SUBFOLDERS)
	cp $(ROOT_DIR)framework/log4cplus.properties $(OUT_PATH)
	cp remoteipaddress.txt $(OUT_PATH)

release: $(OUT_FULLFNAME)
	 $(SUBFOLDERS)
	cp $(ROOT_DIR)framework/log4cplus.properties $(OUT_PATH)	 
	cp remoteipaddress.txt $(OUT_PATH)
	 
clean:
	-rm -rf $(OBJ_FULLDIR)
	-rm -rf $(OUT_FULLDIR)
	$(SUBFOLDERS)

.PHONY: debug release clean
 

ifeq (,$(findstring $(MAKECMDGOALS),dist clean depend doc docs))
-include $(DEPS)
endif
