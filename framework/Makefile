# 
# iviLINK SDK, version 1.1.2
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

include fwconfig.mk

###############################################################################
# Subfolders
define SUBFOLDERS
$(MAKE) -C utils $@
$(MAKE) -C messageProtocols $@
$(MAKE) -C libraries $@
$(MAKE) -C components $@
$(MAKE) -C sharedLibs $@
$(MAKE) -C genericSamples/linux $@
endef

define TEST_SUBFOLDERS
$(MAKE) -C tests $@
endef

export QML_DIR := $(IVILINK_APP_DIR)qml/

###############################################################################
# Rules

debug: GLOBAL_CFLAGS += $(DEBUG_FLAGS)
debug: GLOBAL_CXXFLAGS += $(DEBUG_FLAGS)
debug:
	@echo $(ROOT_DIR)
	@echo $(FW_DIR)
	$(SUBFOLDERS)

release: GLOBAL_CXXFLAGS += $(RELEASE_FLAGS)
release: GLOBAL_CFLAGS += $(RELEASE_FLAGS)
release:
	$(SUBFOLDERS)

clean: 
	$(SUBFOLDERS)
	-rm -Rf docs
	-rm -Rf "$(IVILINK_REPO_DIR)"
	-rm -Rf "$(IVILINK_LIB_DIR)"
	-rm -Rf "$(IVILINK_BIN_DIR)"
	-rm -Rf "$(IVILINK_PROFILE_DIR)"
	-rm -Rf "$(IVILINK_APP_DIR)"
	-rm -Rf "$(RESULT_DIR)"

3rdp_clean:
	$(MAKE) -C 3rd_party $@

doc:
	rm -Rf docs
	mkdir docs
	doxygen

install:

	mkdir -p $(IVILINK_REPO_DIR)
	mkdir -p $(IVILINK_PROFILE_API_REPO_DIR)
	mkdir -p $(IVILINK_PROFILE_REPO_DIR)
	mkdir -p $(IVILINK_SERVICE_REPO_DIR)
	mkdir -p $(IVILINK_APP_REPO_DIR)
	mkdir -p $(IVILINK_LIB_DIR)
	mkdir -p $(IVILINK_BIN_DIR)
	mkdir -p $(IVILINK_PROFILE_DIR)
	mkdir -p $(IVILINK_APP_DIR)
	
	cp -u "$(RESULT_DIR)/framework/utils/sharedLib/libIviLinkUtils.so" "$(IVILINK_LIB_DIR)"
	cp -u "$(RESULT_DIR)/framework/components/ChannelSupervisor/ChannelLib/libIviLinkChannel.so" "$(IVILINK_LIB_DIR)"
	cp -u "$(RESULT_DIR)/framework/sharedLibs/applicationSharedLib/libIviLinkApp.so" "$(IVILINK_LIB_DIR)"
	
	cp -u "$(RESULT_DIR)/framework/components/SystemController/processEntryPoint/$(SYSTEM_CONTROLLER_PROCESS_NAME)" "$(IVILINK_BIN_DIR)"
	cp -u "$(RESULT_DIR)/framework/components/ConnectivityAgent/process/$(CONNECTIVITY_AGENT_PROCESS_NAME)" "$(IVILINK_BIN_DIR)"
	cp -u "$(RESULT_DIR)/framework/components/ProfileManager/process/$(PROFILE_MANAGER_PROCESS_NAME)" "$(IVILINK_BIN_DIR)"
	cp -u "$(RESULT_DIR)/framework/components/ChannelSupervisor/NegotiatorProcess/$(NEGOTIATOR_PROCESS_NAME)" "$(IVILINK_BIN_DIR)"
	cp -u "$(RESULT_DIR)/framework/components/ApplicationManager/process/$(APPLICATION_MANAGER_PROCESS_NAME)" "$(IVILINK_BIN_DIR)"
	cp -u "$(RESULT_DIR)/framework/components/SystemController/watchdog/$(SYSTEM_CONTROLLER_WATCHDOG_PROCESS_NAME)" "$(IVILINK_BIN_DIR)"

	cp -u "$(FW_DIR)log4cplus.properties" "$(IVILINK_BIN_DIR)"

	cp -u "$(FW_DIR)genericSamples/linux/database/ProfileApiDatabase.xml" "$(IVILINK_PROFILE_API_REPO_DIR)"
	cp -u "$(FW_DIR)genericSamples/linux/database/ProfileDatabase.xml" "$(IVILINK_PROFILE_REPO_DIR)"
	
	cp -u "$(FW_DIR)genericSamples/linux/database/HeadUnitServices.xml" "$(IVILINK_SERVICE_REPO_DIR)"
	cp -u "$(FW_DIR)genericSamples/linux/database/BasicSampleService.xml" "$(IVILINK_SERVICE_REPO_DIR)"
	cp -u "$(FW_DIR)genericSamples/linux/database/AuthenticationService.xml" "$(IVILINK_SERVICE_REPO_DIR)"
	
	cp    "$(FW_DIR)genericSamples/linux/database/AppManDatabase.xml" "$(IVILINK_APP_REPO_DIR)"
	
	cp -u "$(RESULT_DIR)/framework/genericSamples/linux/profiles/authentication/libAuthenticationProfileImpl.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_DIR)/framework/genericSamples/linux/profiles/basicSample/libBasicSampleProfileImpl.so" "$(IVILINK_PROFILE_DIR)"
	
	cp -u "$(RESULT_DIR)/framework/genericSamples/linux/applications/basicSample/BasicSample" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_DIR)/framework/genericSamples/linux/applications/authentication/AuthenticationApplication" "$(IVILINK_BIN_DIR)"
	cp -u "$(RESULT_DIR)/framework/genericSamples/linux/applications/deviceManager/DeviceManager" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_DIR)/framework/genericSamples/linux/applications/IVILinkProgressBar/IVILinkProgressBar" "$(IVILINK_BIN_DIR)"

unittest :
	$(TEST_SUBFOLDERS)
	
rununittest : unittest
	$(TEST_SUBFOLDERS)
	
comptest :
	$(TEST_SUBFOLDERS)
	
	cp -u "$(FW_DIR)log4cplus.properties" "$(IVILINK_APP_DIR)"
	cp -u "$(FW_DIR)tests/component/ProfileLibrary/ChannelTest/ProfileTestChannelTest.properties" "$(IVILINK_APP_DIR)"
	
runcomptest :
	$(TEST_SUBFOLDERS)
	
test : unittest comptest
	
runtest : rununittest runcomptest
	
.PHONY: debug release clean 3rdp_clean install test runtest unittest rununittest comptest runcomptest


