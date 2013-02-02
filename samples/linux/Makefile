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

export SAMPLES_DIR := $(abspath $(ROOT_DIR)samples/linux)/
export RESULT_DIR := $(abspath $(SAMPLES_DIR)/products)/
export RESULT_APP_DIR := $(RESULT_DIR)samples/linux/applications/
export RESULT_PROFILE_DIR := $(RESULT_DIR)samples/linux/profiles/

###############################################################################
# Subfolders
define SUBFOLDERS
$(MAKE) -C profiles $@
$(MAKE) -C applications $@
endef
###############################################################################

debug:
	$(SUBFOLDERS)

release:
	$(SUBFOLDERS)
	
install:

	cp -r "$(SAMPLES_DIR)database/" "$(IVILINK_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)ClimateReceiverProfile/libClimateReceiverProfile.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)ClimateSenderProfile/libClimateSenderProfile.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)MediaControlClientProfile/libMediaControlClientProfile.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)MediaControlServerProfile/libMediaControlServerProfile.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)MediaSourceClientProfile/libMediaSourceClientProfile.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)MediaSourceServerProfile/libMediaSourceServerProfile.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)MediaStreamingClientProfile/libMediaStreamingClientProfile.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)MediaStreamingServerProfile/libMediaStreamingServerProfile.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)ScreenSharingProfileImpl/libScreenSharingProfileImpl.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)SeatReceiverProfile/libSeatReceiverProfile.so" "$(IVILINK_PROFILE_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)SeatSenderProfile/libSeatSenderProfile.so" "$(IVILINK_PROFILE_DIR)"
	
	cp -u "$(RESULT_APP_DIR)Climate_App/Climate_App" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_APP_DIR)Climate_App/climateLog.properties" "$(IVILINK_APP_DIR)"
	cp -ur "$(RESULT_APP_DIR)Climate_App/qml" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_APP_DIR)launcher/launcher" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_APP_DIR)launcher/launcherLog.properties" "$(IVILINK_APP_DIR)"
	cp -ur "$(RESULT_APP_DIR)launcher/qml" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_APP_DIR)MediaApp/MediaApp" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_APP_DIR)MediaApp/mediaLog.properties" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)MediaControlServerProfile/MediaServer/MediaServer" "$(IVILINK_APP_DIR)"
	cp -ur "$(RESULT_APP_DIR)MediaApp/qml" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_APP_DIR)ScreenSharingViewer/ScreenSharingViewer" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_APP_DIR)ScreenSharingViewer/screenSharingViewerLog.properties" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_APP_DIR)Seat_App/Seat_App" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_APP_DIR)Seat_App/seatAppLog.properties" "$(IVILINK_APP_DIR)"
	cp -ur "$(RESULT_APP_DIR)Seat_App/qml" "$(IVILINK_APP_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)States/Seat/libSeatState.so" "$(IVILINK_LIB_DIR)"
	cp -u "$(RESULT_PROFILE_DIR)States/Climate/libClimateState.so" "$(IVILINK_LIB_DIR)"
	cp -u "$(SAMPLES_DIR)applications/launcher/restart.sh" "$(IVILINK_APP_DIR)"
	cp -u "$(SAMPLES_DIR)applications/launcher/restart-n.sh" "$(IVILINK_APP_DIR)"
	cp -u "$(SAMPLES_DIR)applications/launcher/shutdown.sh" "$(IVILINK_APP_DIR)"
	cp -u "$(SAMPLES_DIR)applications/launcher/unpair.sh" "$(IVILINK_APP_DIR)"

clean: 
	$(SUBFOLDERS)
	rm -Rf docs

doc:
	rm -Rf docs
	mkdir docs
	doxygen

.PHONY: debug release install clean

