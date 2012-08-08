# 
# iviLINK SDK, version 1.0.1
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

OUT_FNAME     := libiviLink.a

###############################################################################
# Variables

OUT_PATH := $(call OUT_PATH_FUNC)
$(info OUT_PATH $(OUT_PATH))

OUT_FULLDIR   := $(OUT_PATH)
OUT_FULLFNAME := $(OUT_FULLDIR)$(OUT_FNAME)

OBJ_FULLDIR := $(OUT_PATH)$(OBJ_DIR)


###############################################################################

createlib: 
	mkdir -p $(OUT_FULLDIR)
	mkdir -p $(OBJ_FULLDIR)
	ar x $(RESULT_DIR)/utils/configurator/libConfigurator.a
	ar x $(RESULT_DIR)/utils/json/libjson.a
	ar x $(RESULT_DIR)/utils/misc/libUtils.a
	ar x $(RESULT_DIR)/utils/ipc/libIpc.a
	ar x $(RESULT_DIR)/utils/ipc/helpers/libIpcHelpers.a
	ar x $(RESULT_DIR)/utils/threads/libThreads.a
	ar x $(RESULT_DIR)/utils/xml/libXMLParser.a
	ar x $(RESULT_DIR)/utils/serialize/libSerialize.a
	ar x $(RESULT_DIR)/framework/components/ConnectivityAgent/generic/HAL/libHAL.a
	ar x $(RESULT_DIR)/framework/components/ConnectivityAgent/generic/L0/libL0.a
	ar x $(RESULT_DIR)/framework/components/ConnectivityAgent/generic/L1/libL1.a
	ar x $(RESULT_DIR)/framework/components/ChannelSupervisor/common/libmessages.a
	ar x $(RESULT_DIR)/framework/components/ChannelSupervisor/Tube/libtube.a
	ar x $(RESULT_DIR)/framework/components/ChannelSupervisor/NegotiatorClient/libnegotiator_client.a
	ar x $(RESULT_DIR)/framework/components/ProfileManager/PMAL/libPMAL.a
	ar x $(RESULT_DIR)/framework/components/ProfileManager/PMAL/core/libPMAL_core.a
	ar x $(RESULT_DIR)/framework/components/ProfileManager/PMAL/PIM/libPMAL_PIM.a
	ar x $(RESULT_DIR)/framework/components/ProfileManager/PMAL/ipc_protocol/libPMAL_IPC.a
	ar x $(RESULT_DIR)/framework/components/ProfileManager/IpcProtocol/libPM_IpcProtocol.a
	ar x $(RESULT_DIR)/framework/components/ProfileManager/PMAL/ipc_protocol/libPMAL_IPC.a
	ar x $(RESULT_DIR)/framework/libraries/ServiceManager/libServices.a
	ar x $(RESULT_DIR)/framework/libraries/AppMan/App/libAppManApp.a
	ar x $(RESULT_DIR)/framework/public/appLib/libApp.a
	ar x $(RESULT_DIR)/framework/components/ProfileRepository/libProfileRepo.a
	ar x $(RESULT_DIR)/framework/components/ConnectivityAgent/generic/common/libConnectivityAgent.a
	ar x $(RESULT_DIR)/framework/public/profileLib/libProfileLib.a
	mv *.o $(OBJ_FULLDIR)
	ar cr $(OUT_FULLDIR)/$(OUT_FNAME) $(OBJ_FULLDIR)/*.o

debug: createlib

release: createlib

clean:
	-rm -rf $(OUT_FULLDIR)
	-rm -rf $(OBJ_FULLDIR)
	$(SUBFOLDERS)

.PHONY: debug release clean createlib subfolders

ifeq (,$(findstring $(MAKECMDGOALS),dist clean depend doc docs))
-include $(DEPS)
endif
