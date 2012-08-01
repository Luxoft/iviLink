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
define SUBFOLDERS
$(MAKE) -C log4cplus $@
$(MAKE) -C cppunit $@
$(MAKE) -C cryptopp $@
endef
###############################################################################
# Rules
debug:
	@echo $(ROOT_DIR)
	$(SUBFOLDERS)

release:
	$(SUBFOLDERS)
	
clean: 
	$(SUBFOLDERS)
	-rm -Rf $(3RD_PARTY_DIR)

3rdp_clean:
	$(SUBFOLDERS)


.PHONY: debug release clean


