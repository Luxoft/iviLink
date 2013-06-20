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
define COMP_SUBFOLDERS
#$(MAKE) -C framework $@
#$(MAKE) -C utils $@
$(MAKE) -C ProfileLibrary debug
$(MAKE) -C ApplicationLibrary debug
endef
###############################################################################
# Rules
	
comptest :
	$(COMP_SUBFOLDERS)

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
	
	cp -u "$(RESULT_DIR)/framework/tests/component/ProfileLibrary/BufferTest/ProfileLibBufferTest" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_DIR)/framework/tests/component/ProfileLibrary/ChannelTest/Client/ProfileLibChannelTestClient" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_DIR)/framework/tests/component/ProfileLibrary/ChannelTest/Server/ProfileLibChannelTestServer" "$(IVILINK_APP_DIR)"
	cp -u "$(FW_DIR)tests/component/ProfileLibrary/ChannelTest/ProfileLibChannelTest1.sh" "$(IVILINK_APP_DIR)"
	cp -u "$(FW_DIR)tests/component/ProfileLibrary/ChannelTest/ProfileLibChannelTest2.sh" "$(IVILINK_APP_DIR)"
	
	cp -u "$(FW_DIR)components/ConnectivityAgent/1ca.sh" "$(IVILINK_APP_DIR)"
	cp -u "$(FW_DIR)components/ConnectivityAgent/2ca.sh" "$(IVILINK_APP_DIR)"
	cp -u "$(FW_DIR)components/ChannelSupervisor/1neg.sh" "$(IVILINK_APP_DIR)"
	cp -u "$(FW_DIR)components/ChannelSupervisor/2neg.sh" "$(IVILINK_APP_DIR)"
		
runcomptest :
	$(COMP_SUBFOLDERS)
	
.PHONY: comptest runcomptest


