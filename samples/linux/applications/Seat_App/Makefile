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

###############################################################################
# Subfolders
define SUBFOLDERS
endef
###############################################################################
# This target
SRC_DIR := .

RESOURCES_DIR := QTClient/qml
RESOURCES := $(patsubst $(RESOURCES_DIR)/%,%,$(wildcard $(RESOURCES_DIR)/*))

CFLAGS   := -I$(ROOT_DIR) -Wall -pthread -fPIC -I$(3RD_PARTY_RES_DIR)include 
LDFLAGS  := -L$(ROOT_DIR) -lrt -pthread -L$(IVILINK_LIB_DIR) -Wl,-rpath,$(PREFIX)
CFLAGS   += -DUSELOG4CPLUS
LDFLAGS  += -llog4cplus -Wl,-rpath,$(IVILINK_LIB_DIR) -rdynamic
CXXFLAGS := $(CFLAGS)

###############################################################################
# Variables
OUT_PATH := $(call OUT_PATH_FUNC)
$(info OUT_PATH $(OUT_PATH))

QINCLUDES += $(IVILINK_INCLUDE_COMMON_DIR)
QINCLUDES += $(IVILINK_INCLUDE_DIR)ApplicationLibrary/
QINCLUDES += $(3RD_PARTY_SRC_DIR)json/include/
QINCLUDES += $(IVILINK_INCLUDE_UTILS_DIR)misc/
QINCLUDES += $(IVILINK_INCLUDE_UTILS_DIR)threads/
QINCLUDES += $(IVILINK_UTILS_DIR)android/
QINCLUDES += $(SAMPLES_DIR)services/ProfileProxy/
QINCLUDES += $(SAMPLES_DIR)services/ProfileApi/
QINCLUDES += $(SAMPLES_DIR)services/ProfileApi/States/
QINCLUDES += $(SAMPLES_DIR)services/ProfileApi/States/Seat/
QINCLUDES += $(ROOT_DIR)framework/libraries/ApplicationLibrary/ServiceManager/
QINCLUDES += $(ROOT_DIR)framework/libraries/ApplicationLibrary/PMAL/


LIBS += -Wl,--start-group
LIBS += -L$(IVILINK_LIB_DIR) -llog4cplus
LIBS += -L$(IVILINK_LIB_DIR) -lIviLinkUtils
LIBS += -L$(IVILINK_LIB_DIR) -lIviLinkChannel
LIBS += -L$(IVILINK_LIB_DIR) -lIviLinkApp
LIBS += -L$(RESULT_DIR)/samples/linux/profiles/States/Seat -lSeatState
LIBS += -L$(ROOT_DIR)
LIBS += -lrt
LIBS += -Wl,--end-group 
LIBS+= -Wl,-rpath=./


RESOURCES_FULLDIR:=$(OUT_PATH)
OBJ_FULLDIR := $(OUT_PATH)$(OBJ_DIR)
MOC_FULLDIR := $(OBJ_FULLDIR)

MK_FULLDIR := $(OBJ_FULLDIR)

# Sort used to remove duplicates
ALL_OUT_DIRS := $(sort $(OUT_PATH) $(OBJ_FULLDIR) $(MOC_FULLDIR) $(MK_FULLDIR) $(RESOURCES_FULLDIR) $(OUT_PROFILE_REPO_DIR) $(OUT_SERVICE_REPO_DIR) $(OUT_APPMAN_DB_DIR))
$(info ALL_OUT_DIRS $(ALL_OUT_DIRS))
###############################################################################
# Rules

$(ALL_OUT_DIRS):
	mkdir -p $@

#$(RESOURCES_FULLDIR)/%: $(RESOURCES_DIR)/%
#	cp -r '$<' '$@'

#copy_res:
#	cp -r qml $(OUT_PATH)

qmake: $(ALL_OUT_DIRS)
	qmake-qt4 . -r -spec linux-g++ -o $(MK_FULLDIR)/Makefile OBJECTS_DIR=$(OBJ_FULLDIR) MOC_DIR=$(MOC_FULLDIR) DESTDIR=$(OUT_PATH) INCLUDEPATH="$(QINCLUDES)" LIBS="$(LIBS)" QMAKE_LFLAGS="$(LDFLAGS)" QMAKE_CFLAGS="$(CFLAGS)" QMAKE_CXXFLAGS="$(CXXFLAGS)"
	make -C $(MK_FULLDIR)


build: qmake 
	cp *.properties $(OUT_PATH)

debug: build
	$(SUBFOLDERS)

release: build
	$(SUBFOLDERS)

clean:
	-rm -rf $(ALL_OUT_DIRS)
	$(SUBFOLDERS)

.PHONY: debug release clean qmake
