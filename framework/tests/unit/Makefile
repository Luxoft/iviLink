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
define UNIT_SUBFOLDERS
$(MAKE) -C framework $@
endef
###############################################################################
# Rules

unittest :
	$(UNIT_SUBFOLDERS)
	
rununittest :
	$(UNIT_SUBFOLDERS)
	
.PHONY: unittest rununittest


