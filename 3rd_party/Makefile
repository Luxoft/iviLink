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

include 3rdpconfig.mk

###############################################################################
# Subfolders
define SUBFOLDERS
$(MAKE) -C log4cplus $@
$(MAKE) -C cryptopp $@
$(MAKE) -C xml $@
$(MAKE) -C json $@
endef
###############################################################################
# Rules
debug:
	@echo $(ROOT_DIR)
	$(SUBFOLDERS)

release:
	$(SUBFOLDERS)

3rdp_clean:
	$(SUBFOLDERS)
	-rm -Rf $(3RD_PARTY_RES_DIR)
	
install:
	mkdir -p $(IVILINK_LIB_DIR)
	cp -u "$(3RD_PARTY_RES_DIR)lib/liblog4cplus-1.0.so.4" "$(IVILINK_LIB_DIR)"
	cp -u "$(3RD_PARTY_RES_DIR)lib/liblog4cplus.so" "$(IVILINK_LIB_DIR)"

.PHONY: debug release clean3rdp_clean


