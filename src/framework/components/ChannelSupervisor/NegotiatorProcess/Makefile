#  iviLINK SDK, version 0.9 (for preview only)                                      
#    http://www.ivilink.net                                                         
#  Cross Platform Application Communication Stack for In-Vehicle Applications       
#                                                                                   
#  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
#                                                                                   
#  This library is free software; you can redistribute it and/or                    
#  modify it under the terms of the GNU Lesser General Public                       
#  License as published by the Free Software Foundation; version 2.1.               
#                                                                                   
#  This library is distributed in the hope that it will be useful,                  
#  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
#  Lesser General Public License for more details.                                  
#                                                                                   
#  You should have received a copy of the GNU Lesser General Public                 
#  License along with this library; if not, write to the Free Software              
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
# 
# 
# 
# 
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
SRC := $(wildcard $(SRC_DIR)/*.cpp)

OUT_FNAME     := $(NEGOTIATOR_PROCESS_NAME)

INCLUDES      := -Iinclude
CXXFLAGS      := $(GLOBAL_CXXFLAGS) $(INCLUDES)
LDFLAGS       := $(GLOBAL_LDFLAGS)

LIBS := utils/misc/libUtils.a
LIBS += utils/threads/libThreads.a
LIBS += utils/ipc/libIpc.a
LIBS += framework/components/ConnectivityAgent/src/generic/L0/libL0.a
LIBS += framework/components/ChannelSupervisor/NegotiatorClient/libnegotiator_client.a
LIBS += framework/components/ChannelSupervisor/Tube/libtube.a
LIBS += framework/components/ChannelSupervisor/Map/libmap.a
LIBS += framework/components/ChannelSupervisor/common/libmessages.a
LIBS += framework/messageProtocol/SystemController_ChannelSupervisor/ChannelSupervisor/libSystemCtrlMsgs.a

DLIBS :=
DLIBS += framework/components/ConnectivityAgent/src/generic/common/libConnectivityAgent.so


LIBS := $(addprefix $(RESULT_DIR)/,$(LIBS))

DLIBS := $(addprefix $(RESULT_DIR)/,$(DLIBS))
DLIBS_LDFLAGS_l := $(patsubst lib%.so,-l%,$(notdir $(DLIBS)))
DLIBS_LDFLAGS_L := $(addprefix -L,$(dir $(DLIBS))) 
DLIBS_LDFLAGS_RPATH := $(addprefix -Wl$(COMMA)-rpath$(COMMA),$(dir $(DLIBS))) 

###############################################################################
# Variables

OUT_PATH := $(call OUT_PATH_FUNC)
$(info OUT_PATH $(OUT_PATH))

OUT_FULLDIR   := $(OUT_PATH)
OUT_FULLFNAME := $(OUT_FULLDIR)$(OUT_FNAME)


OBJ_FULLDIR := $(OUT_PATH)$(OBJ_DIR)
OBJS        := $(SRC:.cpp=.o)
OBJS        := $(OBJS:.c=.o)
OBJS        := $(addprefix $(OBJ_FULLDIR)/,$(notdir $(OBJS)))
DEPS        := $(OBJS:.o=.d)

OUT_LOG_CONFIG_DIR := $(OUT_FULLDIR)
OUT_LOG_CONFIG := $(OUT_LOG_CONFIG_DIR)/log4cplus.properties

###############################################################################
# Rules
$(OUT_FULLDIR):
	mkdir -p $@

$(OBJ_FULLDIR):
	mkdir -p $@

$(OUT_FULLFNAME): $(OBJS) $(LIBS) $(DLIBS)| $(OUT_FULLDIR) 
	$(CXX) $(OBJS) -Wl,--start-group $(LIBS) $(DLIBS_LDFLAGS_L) $(DLIBS_LDFLAGS_l) -Wl,--end-group $(LDFLAGS) $(DLIBS_LDFLAGS_RPATH) -o $@
	

$(OBJ_FULLDIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_FULLDIR)
	$(CXX) -c $< $(CXXFLAGS) -MMD -o $@

$(OUT_LOG_CONFIG): $(ROOT_DIR)framework/log4cplus.properties
	cp $< $@

log_config: $(OUT_LOG_CONFIG)

debug: $(OUT_FULLFNAME) log_config
	$(SUBFOLDERS)

release: $(OUT_FULLFNAME) log_config
	$(SUBFOLDERS)

clean:
	-rm -rf $(OBJ_FULLDIR)
	-rm -rf $(OUT_FULLDIR)
	$(SUBFOLDERS)

.PHONY: debug release clean

ifeq (,$(findstring $(MAKECMDGOALS),dist clean depend doc docs))
-include $(DEPS)
endif
