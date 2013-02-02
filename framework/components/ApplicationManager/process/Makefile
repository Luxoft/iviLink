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

COMP_APPMAN_DIR	:= $(ROOT_DIR)framework/components/ApplicationManager/

SRC_DIR := .
INCLUDES	     += -I$(IVILINK_INCLUDE_COMMON_DIR)
SRC := $(wildcard $(SRC_DIR)/*.cpp)

OUT_FNAME     := $(APPLICATION_MANAGER_PROCESS_NAME)

FW_DIR		  := $(ROOT_DIR)framework/
LIB_APPMAN_DIR	:= $(ROOT_DIR)framework/messageProtocols/

INCLUDES      += -I.
INCLUDES      += -I$(LIB_APPMAN_DIR)SystemController_ApplicationManager/ApplicationManager/
INCLUDES      += -I$(LIB_APPMAN_DIR)SystemController_ApplicationManager/
INCLUDES      += -I$(LIB_APPMAN_DIR)
INCLUDES      += -I$(COMP_APPMAN_DIR)database/
INCLUDES      += -I$(COMP_APPMAN_DIR)launcher/
INCLUDES      += -I$(COMP_APPMAN_DIR)ports/ubuntu/
INCLUDES      += -I$(LIB_APPMAN_DIR)ApplicationManager_Application/ApplicationManager/
INCLUDES      += -I$(LIB_APPMAN_DIR)ApplicationManager_Application/Application/
INCLUDES      += -I$(LIB_APPMAN_DIR)ApplicationManager_ProfileManager/ApplicationManager/
INCLUDES      += -I$(LIB_APPMAN_DIR)ApplicationManager_ProfileManager/ProfileManager/
INCLUDES      += -I$(FW_DIR)components/ApplicationManager/controller/
INCLUDES      += -I$(FW_DIR)messageProtocols/WatchdogThread/
INCLUDES	  += -I$(FW_DIR)components/SystemController/componentLauncher/
INCLUDES      += -I$(UTILS_DIR)udpSocket/
INCLUDES      += -I$(UTILS_DIR)ipc/
INCLUDES      += -I$(UTILS_DIR)misc/

CXXFLAGS      := $(GLOBAL_CXXFLAGS) $(INCLUDES)
LDFLAGS       := $(GLOBAL_LDFLAGS) -llog4cplus -Wl,-rpath,$(IVILINK_LIB_DIR)

LIBS :=
LIBS += framework/messageProtocols/ApplicationManager_ProfileManager/ApplicationManager/libAmpPmp.a
LIBS += framework/messageProtocols/ApplicationManager_Application/ApplicationManager/libAmpApp.a
LIBS += framework/components/ApplicationManager/database/libDatabase.a
LIBS += framework/components/ApplicationManager/requestHandler/libRequestHandler.a
LIBS += framework/components/ApplicationManager/controller/libAmpCtrl.a
LIBS += framework/components/ApplicationManager/ports/ubuntu/libAmpUbuntuImpls.a
LIBS += framework/messageProtocols/SystemController_ApplicationManager/ApplicationManager/libSystemCtrlMsgs.a

DLIBS := 
DLIBS += framework/utils/sharedLib/libIviLinkUtils.so
DLIBS += framework/components/ChannelSupervisor/ChannelLib/libIviLinkChannel.so

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

debug: $(OUT_FULLFNAME)
	$(SUBFOLDERS)
	cp $(ROOT_DIR)framework/log4cplus.properties $(OUT_FULLDIR)

release: $(OUT_FULLFNAME)
	$(SUBFOLDERS)

clean:
	-rm -rf $(OBJ_FULLDIR)
	-rm -rf $(OUT_FULLDIR)
	$(SUBFOLDERS)

.PHONY: debug release clean
 

ifeq (,$(findstring $(MAKECMDGOALS),dist clean depend doc docs))
-include $(DEPS)
endif
