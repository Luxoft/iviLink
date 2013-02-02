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

OUT_FNAME     := $(SYSTEM_CONTROLLER_PROCESS_NAME)

SYSC_DIR	  := $(ROOT_DIR)framework/components/SystemController/
MSGPROTO_DIR  := $(ROOT_DIR)framework/messageProtocols/

INCLUDES      += -I.
INCLUDES	     += -I$(IVILINK_INCLUDE_COMMON_DIR)
INCLUDES      += -I$(SYSC_DIR)ssm/stateMachine/
INCLUDES      += -I$(SYSC_DIR)componentLauncher/
INCLUDES      += -I$(MSGPROTO_DIR)
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_ConnectivityAgent/SystemController/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_ConnectivityAgent/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_ChannelSupervisor/SystemController/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_ChannelSupervisor/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_ProfileManager/SystemController/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_ProfileManager/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_ApplicationManager/SystemController/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_ApplicationManager/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_Authentication/SystemController/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_Authentication/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_SystemControllerWatchdog/SystemController/
INCLUDES      += -I$(MSGPROTO_DIR)SystemController_SystemControllerWatchdog/
INCLUDES      += -I$(UTILS_DIR)udpSocket/
INCLUDES      += -I$(ROOT_DIR)framework/messageProtocols/WatchdogThread/
INCLUDES      += -I$(UTILS_DIR)/ipc/

CXXFLAGS      := $(GLOBAL_CXXFLAGS) $(INCLUDES)
LDFLAGS       := $(GLOBAL_LDFLAGS) -llog4cplus -Wl,-rpath,$(IVILINK_LIB_DIR)

LIBS := framework/components/SystemController/ssm/stateMachine/libSSM.a
LIBS += framework/components/SystemController/ssm/states/libSSMStates.a
LIBS += framework/components/SystemController/componentLauncher/libComponentLauncher.a
LIBS += framework/messageProtocols/SystemController_ConnectivityAgent/SystemController/libConnectivityAgntMsgs.a
LIBS += framework/messageProtocols/SystemController_ChannelSupervisor/SystemController/libChannelSprvsrMsgs.a
LIBS += framework/messageProtocols/SystemController_ProfileManager/SystemController/libProfileMngrMsgs.a
LIBS += framework/messageProtocols/SystemController_ApplicationManager/SystemController/libAppManMsgs.a
LIBS += framework/messageProtocols/SystemController_Authentication/SystemController/libAuthenticationAppMsgs.a
LIBS += framework/messageProtocols/SystemController_SystemControllerWatchdog/SystemController/libSystemControllerToWatchdog.a
LIBS += framework/utils/threads/libThreads.a
LIBS += framework/utils/misc/libUtils.a
LIBS += framework/utils/ipc/libIpc.a
LIBS += framework/utils/ipc/helpers/libIpcHelpers.a
LIBS += framework/utils/udpSocket/libUdpSocket.a
LIBS += framework/utils/tcpSocket/libTcpSocket.a
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
	$(CXX) $(OBJS) -Wl,--start-group $(LIBS) -Wl,--end-group $(LDFLAGS) -o $@


$(OBJ_FULLDIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_FULLDIR)
	$(CXX) -c $< $(CXXFLAGS) -MMD -o $@

debug:  $(OUT_FULLFNAME)
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
