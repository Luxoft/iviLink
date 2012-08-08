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
$(MAKE) -C Profiles $@
$(MAKE) -C BasicSample $@
$(MAKE) -C AuthenticationApplication $@
$(MAKE) -C ClimateClient_App $@
$(MAKE) -C ClimateHU_App $@
$(MAKE) -C SeatHU_App $@
$(MAKE) -C SeatClient_App $@
endef
###############################################################################
# This target
SRC_DIR := .

RESOURCES_DIR := QTClient/qml
RESOURCES := $(patsubst $(RESOURCES_DIR)/%,%,$(wildcard $(RESOURCES_DIR)/*))
LDFLAGS := $(GLOBAL_LDFLAGS) -rdynamic
#LDFLAGS := 
CFLAGS := $(GLOBAL_CFLAGS)
CXXFLAGS := $(GLOBAL_CXXFLAGS)


PROFILE_REPO_DIR := $(SRC_DIR)/Profiles/xml
PROFILE_REPO_XMLS := $(wildcard $(PROFILE_REPO_DIR)/*.xml)
$(info PROFILE_REPO_DIR $(PROFILE_REPO_DIR))
$(info PROFILE_REPO_XMLS $(PROFILE_REPO_XMLS))

SERVICE_REPO_DIR := $(ROOT_DIR)/framework/libraries/ServicesLib/src/xml
SERVICE_REPO_XMLS := $(wildcard $(SERVICE_REPO_DIR)/*.xml)

APPMAN_DB_DIR := $(SRC_DIR)
APPMAN_DB_XML := $(APPMAN_DB_DIR)/AppManDatabase.xml

###############################################################################
# Variables
OUT_PATH := $(call OUT_PATH_FUNC)
$(info OUT_PATH $(OUT_PATH))

QINCLUDES := $(ROOT_DIR) $(RESULT_DIR)

LIBS += -Wl,--start-group
LIBS += -L$(3RD_PARTY_DIR)/lib -llog4cplus
LIBS += -L$(RESULT_DIR)/framework/appLib/staticLib -liviLink
LIBS += -lrt
LIBS += -Wl,--end-group 
LIBS+= -Wl,-rpath=./

RESOURCES_FULLDIR:=$(OUT_PATH)
OBJ_FULLDIR := $(OUT_PATH)$(OBJ_DIR)
MOC_FULLDIR := $(OBJ_FULLDIR)

MK_FULLDIR := $(OBJ_FULLDIR)

OUT_PROFILE_REPO_DIR := $(RESULT_DIR)/framework/components/ProfileManager/PMP/process
OUT_PROFILE_REPO_XMLS := $(addprefix $(OUT_PROFILE_REPO_DIR)/,$(notdir $(PROFILE_REPO_XMLS)))
$(info OUT_PROFILE_REPO_XMLS $(OUT_PROFILE_REPO_XMLS))

OUT_SERVICE_REPO_DIR := $(OUT_PATH)/xml
OUT_SERVICE_REPO_XMLS := $(addprefix $(OUT_SERVICE_REPO_DIR)/,$(notdir $(SERVICE_REPO_XMLS)))

OUT_APPMAN_DB_DIR := $(RESULT_DIR)/framework/components/AppMan/process
OUT_APPMAN_DB_XML := $(addprefix $(OUT_APPMAN_DB_DIR)/,$(notdir $(APPMAN_DB_XML)))

# Sort used to remove duplicates
ALL_OUT_DIRS := $(sort $(OUT_PATH) $(OBJ_FULLDIR) $(MOC_FULLDIR) $(MK_FULLDIR) $(RESOURCES_FULLDIR) $(OUT_PROFILE_REPO_DIR) $(OUT_SERVICE_REPO_DIR) $(OUT_APPMAN_DB_DIR))
$(info ALL_OUT_DIRS $(ALL_OUT_DIRS))
###############################################################################
# Rules

$(ALL_OUT_DIRS):
	mkdir -p $@

$(RESOURCES_FULLDIR)/%: $(RESOURCES_DIR)/%
	cp -r '$<' '$@'

$(OUT_PROFILE_REPO_DIR)/%.xml: $(PROFILE_REPO_DIR)/%.xml | $(OUT_PROFILE_REPO_DIR)
	cp $< $@

$(OUT_SERVICE_REPO_DIR)/%.xml: $(SERVICE_REPO_DIR)/%.xml | $(OUT_SERVICE_REPO_DIR)
	cp $< $@

$(OUT_APPMAN_DB_DIR)/%.xml: $(APPMAN_DB_DIR)/%.xml | $(OUT_APPMAN_DB_DIR)
	cp $< $@

#copy_res: $(RESOURCES_OUT_FULL)
copy_res: 
	cp -r qml $(OUT_PATH)

qmake: $(ALL_OUT_DIRS)
#	qmake-qt4 . -r -spec linux-g++ -o $(MK_FULLDIR)/Makefile OBJECTS_DIR=$(OBJ_FULLDIR) MOC_DIR=$(MOC_FULLDIR) DESTDIR=$(OUT_PATH) INCLUDEPATH="$(QINCLUDES)" LIBS="$(LIBS)" QMAKE_LFLAGS="$(LDFLAGS)" QMAKE_CFLAGS="$(CFLAGS)" QMAKE_CXXFLAGS="$(CXXFLAGS)"
	make -C $(MK_FULLDIR)

xml_profile_repo: $(OUT_PROFILE_REPO_XMLS)

xml_service_repo: $(OUT_SERVICE_REPO_XMLS)

xml_appman_db: $(OUT_APPMAN_DB_XML)

build: qmake copy_res xml_profile_repo xml_service_repo xml_appman_db
	cp $(ROOT_DIR)framework/log4cplus.properties $(OUT_PATH)

debug: #build
	$(SUBFOLDERS)

release: #build
	$(SUBFOLDERS)

.PHONY: debug release clean qmake xml_profile_repo xml_service_repo

