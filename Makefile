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
include config.mk

###############################################################################
# Subfolders
define SUBFOLDERS
endef
###############################################################################
# Rules

debug:
	$(MAKE) -C 3rd_party $@
	$(MAKE) -C 3rd_party install
	$(MAKE) -C framework $@
	$(MAKE) -C framework install
	$(MAKE) -C samples $@
	
release:
	$(MAKE) -C 3rd_party $@
	$(MAKE) -C 3rd_party install
	$(MAKE) -C framework $@
	$(MAKE) -C framework install
	$(MAKE) -C samples $@

install:
	$(MAKE) -C 3rd_party $@
	$(MAKE) -C framework $@
	$(MAKE) -C samples $@
	
clean:
	$(MAKE) -C framework $@
	$(MAKE) -C samples $@
	
3rdp_clean:
	$(MAKE) -C 3rd_party $@

sdk:
	$(MAKE) -C 3rd_party debug
	$(MAKE) -C 3rd_party install
	$(MAKE) -C framework debug

sdk_debug:
	$(MAKE) -C 3rd_party debug
	$(MAKE) -C 3rd_party install
	$(MAKE) -C framework debug

sdk_release:
	$(MAKE) -C 3rd_party install
	$(MAKE) -C 3rd_party release
	$(MAKE) -C framework release

sdk_install:
	$(MAKE) -C framework install
	
sdk_clean:
	$(MAKE) -C framework clean

samples:
	$(MAKE) -C samples debug

samples_debug:
	$(MAKE) -C samples debug

samples_release:
	$(MAKE) -C samples release

samples_install:
	$(MAKE) -C samples install

unittest:
	$(MAKE) -C framework $@
	
comptest:
	$(MAKE) -C framework $@

clean_results:
	-rm -Rf "$(IVILINK_DIR)applications"
	-rm -Rf "$(IVILINK_DIR)bin"
	-rm -Rf "$(IVILINK_DIR)database"
	-rm -Rf "$(IVILINK_DIR)lib"
	-rm -Rf "$(IVILINK_DIR)profiles"

doc:
	rm -Rf docs
	mkdir docs
	doxygen


.PHONY: sdk_debug sdk_release sdk sdk_install samples_debug samples_release samples samples_install debug release install clean doc
 

