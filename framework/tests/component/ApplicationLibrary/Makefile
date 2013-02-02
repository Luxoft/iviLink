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
$(MAKE) -C AppLibTestUtils $@
$(MAKE) -C AppLibTest $@
$(MAKE) -C AppLibTestUtilsTest $@
$(MAKE) -C AppLibTestProfile $@
endef
###############################################################################
# Rules
debug:
	@echo $(ROOT_DIR)
	$(SUBFOLDERS)

	-rm -Rf "$(IVILINK_REPO_DIR)"
	-rm -Rf "$(IVILINK_PROFILE_DIR)"
	-rm -Rf "$(IVILINK_APP_DIR)"
	
	mkdir -p $(IVILINK_REPO_DIR)
	mkdir -p $(IVILINK_PROFILE_API_REPO_DIR)
	mkdir -p $(IVILINK_PROFILE_REPO_DIR)
	mkdir -p $(IVILINK_SERVICE_REPO_DIR)
	mkdir -p $(IVILINK_APP_REPO_DIR)
	mkdir -p $(IVILINK_PROFILE_DIR)
	mkdir -p $(IVILINK_APP_DIR)
	
	cp -u "$(FW_DIR)log4cplus.properties" "$(IVILINK_BIN_DIR)"

	cp -u "$(FW_DIR)tests/component/ApplicationLibrary/database/ProfileApiDatabase.xml" "$(IVILINK_PROFILE_API_REPO_DIR)"
	cp -u "$(FW_DIR)tests/component/ApplicationLibrary/database/AppLibTestProfile_API.xml" "$(IVILINK_PROFILE_API_REPO_DIR)"
	
	cp -u "$(FW_DIR)tests/component/ApplicationLibrary/database/ProfileDatabase.xml" "$(IVILINK_PROFILE_REPO_DIR)"
	cp -u "$(FW_DIR)tests/component/ApplicationLibrary/database/AppLibTestProfile_Impl.xml" "$(IVILINK_PROFILE_REPO_DIR)"
	
	cp -u "$(FW_DIR)tests/component/ApplicationLibrary/database/HeadUnitServices.xml" "$(IVILINK_SERVICE_REPO_DIR)"
	cp -u "$(FW_DIR)tests/component/ApplicationLibrary/database/AppLibTestService_1.xml" "$(IVILINK_SERVICE_REPO_DIR)"
	cp -u "$(FW_DIR)tests/component/ApplicationLibrary/database/AppLibTestService_7.xml" "$(IVILINK_SERVICE_REPO_DIR)"
	cp -u "$(FW_DIR)tests/component/ApplicationLibrary/database/AppLibTestService_9.xml" "$(IVILINK_SERVICE_REPO_DIR)"
	
	cp -u "$(FW_DIR)tests/component/ApplicationLibrary/database/AppManDatabase.xml" "$(IVILINK_APP_REPO_DIR)"
	
	
	cp -u "$(RESULT_DIR)/framework/tests/component/ApplicationLibrary/AppLibTest/AppLibTest7/AppLibTest7" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_DIR)/framework/tests/component/ApplicationLibrary/AppLibTest/AppLibTest9/AppLibTest9" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_DIR)/framework/tests/component/ApplicationLibrary/AppLibTest/AppLibTest7/remoteipaddress.txt" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_DIR)/framework/tests/component/ApplicationLibrary/AppLibTest/AppLibTest9/remoteipaddress.txt" "$(IVILINK_APP_DIR)"

release:
	$(SUBFOLDERS)
	
clean: 
	$(SUBFOLDERS)


.PHONY: debug release clean
 

