#   This file is part of iviLINK 0.9 Technology Preview Samples Set.          
#                                                                             
#   iviLINK 0.9 Technology Preview Samples Set is free software:              
#   you can redistribute it and/or modify                                     
#   it under the terms of the GNU General Public License as published by      
#   the Free Software Foundation, version 3 of the License.                   
#                                                                             
#   iviLINK 0.9 Technology Preview Samples Set is distributed in the          
#   hope that it will be useful, but WITHOUT ANY WARRANTY; without            
#   even the implied warranty of MERCHANTABILITY or FITNESS FOR A             
#   PARTICULAR PURPOSE.  See the GNU General Public License for more details. 
#                                                                              
#   You should have received a copy of the GNU General Public License         
#   along with iviLINK 0.9 Technology Preview.                                
#   If not, see <http://www.gnu.org/licenses/>.                               
# 
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
$(MAKE) -C Profiles $@
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
LIBS += -L$(RESULT_DIR)/utils/json/ -ljson
LIBS += -L$(3RD_PARTY_DIR)/lib -llog4cplus
LIBS += -L$(RESULT_DIR)/utils/misc -lUtils
LIBS += -L$(RESULT_DIR)/utils/ipc -lIpc
LIBS += -L$(RESULT_DIR)/utils/threads -lThreads
LIBS += -L$(RESULT_DIR)/framework/components/ConnectivityAgent/src/generic/HAL -lHAL
LIBS += -L$(RESULT_DIR)/framework/components/ConnectivityAgent/src/generic/L0 -lL0
LIBS += -L$(RESULT_DIR)/framework/components/ChannelSupervisor/common/ -lmessages
LIBS += -L$(RESULT_DIR)/framework/components/ConnectivityAgent/src/generic/common -lConnectivityAgent
LIBS += -L$(RESULT_DIR)/framework/components/ChannelSupervisor/Tube/ -ltube
LIBS += -L$(RESULT_DIR)/framework/components/ConnectivityAgent/src/generic/common -lConnectivityAgent
LIBS += -L$(RESULT_DIR)/framework/components/ChannelSupervisor/NegotiatorClient/ -lnegotiator_client
LIBS += -L$(RESULT_DIR)/framework/components/ProfileManager/PMAL/ -lPMAL
LIBS += -L$(RESULT_DIR)/framework/components/ProfileManager/PMAL/core/ -lPMAL_core
LIBS += -L$(RESULT_DIR)/framework/components/ProfileManager/PMAL/PIM/ -lPMAL_PIM
LIBS += -L$(RESULT_DIR)/framework/components/ProfileManager/PMAL/ipc_protocol/ -lPMAL_IPC
LIBS += -L$(RESULT_DIR)/framework/components/ProfileManager/IpcProtocol/ -lPM_IpcProtocol
LIBS += -L$(RESULT_DIR)/framework/components/ProfileManager/PMAL/ipc_protocol/ -lPMAL_IPC
LIBS += -L$(RESULT_DIR)/framework/libraries/ServicesLib/src/ -lServices
LIBS += -L$(RESULT_DIR)/framework/libraries/AppMan/App/ -lAppManApp
LIBS += -L$(RESULT_DIR)/framework/components/ProfileRepository/ -lProfileRepo
LIBS += -L$(RESULT_DIR)/utils/configurator -lConfigurator
LIBS += -lrt
LIBS += -Wl,--end-group 
LIBS+= -Wl,-rpath=./
# LDFLAGS := $(LIBS) $(LDFLAGS)
# LIBS :=

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
	qmake-qt4 .  -r -spec linux-g++ -o $(MK_FULLDIR)/Makefile OBJECTS_DIR=$(OBJ_FULLDIR) MOC_DIR=$(MOC_FULLDIR) DESTDIR=$(OUT_PATH) INCLUDEPATH="$(QINCLUDES)" LIBS="$(LIBS)" QMAKE_LFLAGS="$(LDFLAGS)" QMAKE_CFLAGS="$(CFLAGS)" QMAKE_CXXFLAGS="$(CXXFLAGS)"
	make -C $(MK_FULLDIR)


xml_profile_repo: $(OUT_PROFILE_REPO_XMLS)

xml_service_repo: $(OUT_SERVICE_REPO_XMLS)

xml_appman_db: $(OUT_APPMAN_DB_XML)

build: qmake copy_res xml_profile_repo xml_service_repo xml_appman_db
	cp $(ROOT_DIR)framework/log4cplus.properties $(OUT_PATH)
	cp $(RESULT_DIR)/framework/components/ConnectivityAgent/src/generic/common/libConnectivityAgent.so $(OUT_PATH)

debug: build
	$(SUBFOLDERS)

release: build
	$(SUBFOLDERS)

clean:
	-rm -rf $(ALL_OUT_DIRS)
	$(SUBFOLDERS)


.PHONY: debug release clean qmake xml_profile_repo xml_service_repo

