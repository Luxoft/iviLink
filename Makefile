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
include config.mk

###############################################################################
# Subfolders
define SUBFOLDERS

$(MAKE) -C 3rd_party $@
$(MAKE) -C utils $@
$(MAKE) -C framework $@
$(MAKE) -C samples $@
endef

QML_DIR := $(PREFIX)/qml

###############################################################################
# Rules
debug:
	@echo $(ROOT_DIR)
	$(SUBFOLDERS)

release:
	$(SUBFOLDERS)

clean: 
	$(SUBFOLDERS)
	-rm -Rf docs
	-rm -Rf "$(RESULT_DIR)"
	-rm -Rf "$(PREFIX)"

3rdp_clean:
	$(MAKE) -C 3rd_party $@

doc:
	rm -Rf docs
	mkdir docs
	doxygen

install:
	mkdir -p $(PREFIX)
	mkdir -p $(PROFILE_REPOSITORY)
	mkdir -p $(SERVICE_REPOSITORY)
	mkdir -p $(APPLICATION_MANAGER_REPOSITORY)
	mkdir -p $(QML_DIR)
	
	cp -u "$(RESULT_DIR)/3rd_party/lib/liblog4cplus.so" "$(PREFIX)"
	
	cp -u "$(RESULT_DIR)/framework/components/SystemController/processEntryPoint/$(SYSTEM_CONTROLLER_PROCESS_NAME)" "$(PREFIX)"
	cp -u "$(RESULT_DIR)/framework/components/ConnectivityAgent/linux/$(CONNECTIVITY_AGENT_PROCESS_NAME)" "$(PREFIX)"
	cp -u "$(RESULT_DIR)/framework/components/ProfileManager/PMP/process/$(PROFILE_MANAGER_PROCESS_NAME)" "$(PREFIX)"
	cp -u "$(RESULT_DIR)/framework/components/ChannelSupervisor/NegotiatorProcess/$(NEGOTIATOR_PROCESS_NAME)" "$(PREFIX)"
	cp -u "$(RESULT_DIR)/framework/components/AppMan/process/$(APPLICATION_MANAGER_PROCESS_NAME)" "$(PREFIX)"
	cp -u "$(RESULT_DIR)/framework/components/ProfileRepository/process/$(PROFILE_REPOSITORY_PROCESS_NAME)" "$(PREFIX)"
	cp -u "$(ROOT_DIR)/framework/log4cplus.properties" "$(PREFIX)"

	cp -u "$(RESULT_DIR)/utils/shared_lib/libIviLinkUtils.so" "$(PREFIX)"
	cp -u "$(RESULT_DIR)/framework/components/ChannelSupervisor/ChannelLib/libIviLinkChannel.so" "$(PREFIX)"
	cp -u "$(RESULT_DIR)/framework/public/appLib/sharedLib/libIviLinkApp.so" "$(PREFIX)"
	
	cp -u "$(ROOT_DIR)framework/libraries/ServiceManager/xml/"*.xml "$(SERVICE_REPOSITORY)"
	cp -u "$(ROOT_DIR)/samples/linux/Profiles/xml/"*.xml "$(PROFILE_REPOSITORY)"
	cp -u "$(ROOT_DIR)/samples/linux/AppManDatabase.xml" "$(APPLICATION_MANAGER_REPOSITORY)"
	
	cp -ur "$(RESULT_DIR)/samples/linux/Profiles/" "$(PREFIX)"

	cp -u "$(RESULT_DIR)/samples/linux/BasicSample/BasicSample" "$(PREFIX)"
	cp -u "$(RESULT_DIR)/samples/linux/ClimateClient_App/ClimateClient_App" "$(PREFIX)"
	cp -u "$(RESULT_DIR)/samples/linux/ClimateHU_App/ClimateHU_App" "$(PREFIX)"
	cp -ur "$(RESULT_DIR)/samples/linux/ClimateClient_App/obj/qml/ClimateClient_App" "$(QML_DIR)"
	cp -ur "$(RESULT_DIR)/samples/linux/ClimateHU_App/obj/qml/ClimateHU_App" "$(QML_DIR)"
	cp -u "$(RESULT_DIR)/samples/linux/SeatHU_App/SeatHU_App" "$(PREFIX)"
	cp -ur "$(RESULT_DIR)/samples/linux/SeatHU_App/obj/qml/SeatHU_App" "$(QML_DIR)"
	cp -u "$(RESULT_DIR)/samples/linux/SeatClient_App/SeatClient_App" "$(PREFIX)"
	cp -ur "$(RESULT_DIR)/samples/linux/SeatClient_App/obj/qml/SeatClient_App" "$(QML_DIR)"
	cp -u "$(RESULT_DIR)/samples/linux/AuthenticationApplication/AuthenticationApplication" "$(PREFIX)"

.PHONY: debug release clean 3rdp_clean install


