###############################################################################
#                                                                             #
#   This file is part of iviLINK 0.9 Technology Preview Samples Set.          #
#                                                                             #
#   iviLINK 0.9 Technology Preview Samples Set is free software:              #
#   you can redistribute it and/or modify                                     #
#   it under the terms of the GNU General Public License as published by      #
#   the Free Software Foundation, version 3 of the License.                   #
#                                                                             #
#   iviLINK 0.9 Technology Preview Samples Set is distributed in the          #
#   hope that it will be useful, but WITHOUT ANY WARRANTY; without            #
#   even the implied warranty of MERCHANTABILITY or FITNESS FOR A             #
#   PARTICULAR PURPOSE.  See the GNU General Public License for more details. #
#                                                                             #
#   You should have received a copy of the GNU General Public License         #
#   along with iviLINK 0.9 Technology Preview.                                #
#   If not, see <http://www.gnu.org/licenses/>.                               #
#                                                                             #
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
$(MAKE) -C linux $@
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
	rm -Rf docs

doc:
	rm -Rf docs
	mkdir docs
	doxygen



.PHONY: debug release clean
