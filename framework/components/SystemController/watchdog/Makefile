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

SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cpp)
SYSC_DIR	  := $(ROOT_DIR)framework/components/SystemController/
LIBS_DIR	  := $(ROOT_DIR)framework/libraries/

OUT_FNAME     := $(SYSTEM_CONTROLLER_WATCHDOG_PROCESS_NAME)

INCLUDES      := -I.
INCLUDES	  += -I$(IVILINK_INCLUDE_COMMON_DIR)
INCLUDES      += -I$(UTILS_DIR)ipc/
INCLUDES      += -I$(SYSC_DIR)ssm/stateMachine/
INCLUDES      += -I$(LIBS_DIR)TriggerQueue/
INCLUDES      += -I$(SYSC_DIR)componentLauncher/
INCLUDES      += -I$(SYSC_DIR)processEntryPoint/
INCLUDES      += -I$(ROOT_DIR)framework/utils/misc/
INCLUDES      += -I$(ROOT_DIR)framework/utils/threads/
INCLUDES      += -I$(ROOT_DIR)framework/messageProtocols/SystemController_SystemControllerWatchdog/SystemControllerWatchdog/
INCLUDES      += -I$(ROOT_DIR)framework/messageProtocols/SystemController_SystemControllerWatchdog/
INCLUDES      += -I$(ROOT_DIR)framework/messageProtocols/
INCLUDES      += -I$(ROOT_DIR)framework/components/SystemController/watchdog/ports/ubuntu
INCLUDES      += -I$(UTILS_DIR)tcpSocket/
INCLUDES      += -I$(UTILS_DIR)udpSocket/


CXXFLAGS      := $(GLOBAL_CXXFLAGS) $(INCLUDES)
LDFLAGS       := $(GLOBAL_LDFLAGS) -llog4cplus -Wl,-rpath,$(IVILINK_LIB_DIR)


LIBS += framework/messageProtocols/SystemController_SystemControllerWatchdog/SystemControllerWatchdog/libWatchdogToSystemController.a
LIBS += framework/components/SystemController/watchdog/ports/ubuntu/libSystemWatchdogUbuntuPort.a
LIBS += framework/utils/misc/libUtils.a
LIBS += framework/utils/ipc/libIpc.a
LIBS += framework/utils/threads/libThreads.a
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
	$(info O_FNAME $@)

$(OUT_FULLFNAME): $(OBJS) $(LIBS) | $(OUT_FULLDIR) 
	$(CXX) $(OBJS) -Wl,--start-group $(LIBS) -Wl,--end-group $(LDFLAGS) -o $@
	$(info SYSTEM_CONTROLLER_WATCHDOG_PROCESS_NAME $(SYSTEM_CONTROLLER_WATCHDOG_PROCESS_NAME))

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
