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
SRC := 

OUT_FNAME     := libIviLinkUtils.so

INCLUDES      := 

export OBJ_DIR := obj

CFLAGS        := $(GLOBAL_CFLAGS)
CXXFLAGS      := $(GLOBAL_CXXFLAGS) $(INCLUDES)
LDFLAGS       := $(GLOBAL_LDFLAGS)

LIBS := framework/utils/ipc/libIpc.a
LIBS += framework/utils/ipc/helpers/libIpcHelpers.a
LIBS += framework/utils/threads/libThreads.a
LIBS += framework/utils/misc/libUtils.a
LIBS += framework/utils/configurator/libConfigurator.a
LIBS += framework/utils/serialize/libSerialize.a
LIBS += framework/utils/eventHandler/libEventHandler.a
LIBS += framework/utils/udpSocket/libUdpSocket.a
LIBS += framework/utils/tcpSocket/libTcpSocket.a

LIBS := $(call GET_DEPS,$(LIBS))

LIBS += $(3RD_PARTY_RES_DIR)lib/libXMLParser.a 
LIBS += $(3RD_PARTY_RES_DIR)lib/libjson.a


###############################################################################
# Variables

OUT_PATH := $(call OUT_PATH_FUNC)
$(info OUT_PATH $(OUT_PATH))

OUT_FULLDIR   := $(OUT_PATH)
OUT_FULLFNAME := $(OUT_FULLDIR)$(OUT_FNAME)


###############################################################################
# Rules
$(OUT_FULLDIR):
	mkdir -p $@

$(OUT_FULLFNAME): $(LIBS) Makefile | $(OUT_FULLDIR) 
	$(CXX) -shared -Wl,--whole-archive $(LIBS) -Wl,--no-whole-archive $(LDFLAGS) -o $@

log_config: $(OUT_LOG_CONFIG)

debug: $(OUT_FULLFNAME)
	$(SUBFOLDERS)

release: $(OUT_FULLFNAME)
	$(SUBFOLDERS)

clean:
	-rm -rf $(OUT_FULLDIR)
	$(SUBFOLDERS)

.PHONY: debug release clean
 

ifeq (,$(findstring $(MAKECMDGOALS),dist clean depend doc docs))
-include $(DEPS)
endif
