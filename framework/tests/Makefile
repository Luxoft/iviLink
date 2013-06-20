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
$(MAKE) -C unit $@
endef

define COMP_SUBFOLDERS
$(MAKE) -C component $@
endef
###############################################################################
# Rules

unittest :
	$(UNIT_SUBFOLDERS)
	
rununittest : unittest
	$(UNIT_SUBFOLDERS)
	
comptest :
	$(COMP_SUBFOLDERS)
	
runcomptest :
	$(COMP_SUBFOLDERS)
	
.PHONY: unittest rununittest comptest runcomptest


