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

SRC_DIR := private
SRC := $(wildcard $(SRC_DIR)/*.cpp)

OUT_FNAME     := libXMLParser.a

INCLUDES      := -I.
CXXFLAGS      := $(3RDPGLOBAL_CXXFLAGS) $(INCLUDES)

###############################################################################
# Variables
$(info result_dir = $(RESULT_DIR))
OUT_PATH := $(3RD_PARTY_RES_DIR)
$(info OUT_PATH $(OUT_PATH))

OUT_FULLDIR   := $(OUT_PATH)lib/
OUT_FULLFNAME := $(OUT_FULLDIR)$(OUT_FNAME)


OBJ_FULLDIR := $(OUT_PATH)xml_obj/
OBJS        := $(SRC:.cpp=.o)
OBJS        := $(OBJS:.c=.o)
OBJS        := $(addprefix $(OBJ_FULLDIR)/,$(notdir $(OBJS)))
DEPS        := $(OBJS:.o=.d)


###############################################################################
# Rules
$(OUT_FULLDIR):
	mkdir -p $@

$(OBJ_FULLDIR):
	mkdir -p $@

$(OUT_FULLFNAME): $(OBJS) | $(OUT_FULLDIR) 
	ar cr $@ $^

$(OBJ_FULLDIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_FULLDIR)
	$(CXX) -c $< $(CXXFLAGS) -MMD -o $@

debug: $(OUT_FULLFNAME)
	$(SUBFOLDERS)

release: $(OUT_FULLFNAME)
	$(SUBFOLDERS)
	
3rdp_clean:
	-rm -rf $(OUT_FULLDIR)
	-rm -rf $(OBJ_FULLDIR)
	$(SUBFOLDERS)

.PHONY: debug release 3rdp_clean
 

ifeq (,$(findstring $(MAKECMDGOALS),dist clean depend doc docs))
-include $(DEPS)
endif
