#------------------------------------------------------------------------------
# First rule - will be default
#------------------------------------------------------------------------------
release:


#------------------------------------------------------------------------------
# Function uses to return the word before last in the list
#
# Function: PRELAST_WORD
# Arguments: 1: list of words
# Returns: word before last in the list
# Usage: $(call PRELAST_WORD, $(MAKEFILE_LIST))
#------------------------------------------------------------------------------
PRELAST_WORD = $(lastword $(subst $(lastword $(1)),,$(1)))

#------------------------------------------------------------------------------
# Function returns current working dir. Path is ended by '/' symbol.
#
# Function: CURRENT_DIR_FUNC
# Arguments: 
# Returns: current working dir
# Usage: MY_PATH := $(call CURRENT_DIR_FUNC)
#------------------------------------------------------------------------------
CURRENT_DIR_FUNC = $(realpath $(shell pwd))/

#------------------------------------------------------------------------------
# Returns OUT_PATH, where all outpust must be located.
#
# Function: OUT_PATH_FUNC
# Arguments: 
# Returns: OUT_PATH
# Usage: OUT_PATH := $(call OUT_PATH_FUNC)
#------------------------------------------------------------------------------
OUT_PATH_FUNC = $(RESULT_DIR)/$(subst $(ROOT_DIR),,$(call CURRENT_DIR_FUNC))


###############################################################################
# Library helpers
###############################################################################


#------------------------------------------------------------------------------
# Returns list of paths to libraries/some files. Tjis list can be used for
# specifing target dependencies.
# $(RESULT_DIR) must be specified before using this function
#
# Function: GET_DEPS
# Arguments: 1: list of files with paths relative to $(RESULT_DIR)
# Returns: list of paths to files
# Usage: 
# LIBS := path/to/ABC/libABC.so
# LIBS += path/to/QWE/libQWE.so
# LIBS_DEPS := $(call GET_DEPS, $(LIBS))
#------------------------------------------------------------------------------
GET_DEPS = $(addprefix $(RESULT_DIR)/,$(1))

#------------------------------------------------------------------------------
# Uses list of dynamic libraries to build -l options for ldflags.
#
# Function: GET_DYNLIB_LDFLAGS_l
# Arguments: 1: list of dynamic libraries 
# Returns: linker's -l options
# Usage: 
# DLIBS := path/to/ABC/libABC.so
# DLIBS += path/to/QWE/libQWE.so
# LDFLAGS += $(call GET_DYNLIB_LDFLAGS_l, $(DLIBS))  # will be -lABC -lQWE
#------------------------------------------------------------------------------
GET_DYNLIB_LDFLAGS_l = $(patsubst lib%.so,-l%,$(notdir $(1)))

#------------------------------------------------------------------------------
# Uses list of dynamic libraries to build -L options for ldflags.
#
# Function: GET_DYNLIB_LDFLAGS_L
# Arguments: 1: list of dynamic libraries 
# Returns: linker's -L options
# Usage: 
# DLIBS := path/to/ABC/libABC.so
# DLIBS += path/to/QWE/libQWE.so
# LDFLAGS += $(call GET_DYNLIB_LDFLAGS_L, $(DLIBS))  
## will be -Lpath/to/ABC -Lpath/to/QWE
#------------------------------------------------------------------------------
GET_DYNLIB_LDFLAGS_L = $(addprefix -L,$(dir $(call GET_DEPS,$(1))))

#------------------------------------------------------------------------------
# Uses list of dynamic libraries to build rpath for ldflags.
#
# Function: GET_DYNLIB_LDFLAGS_RPATH
# Arguments: 1: list of dynamic libraries 
# Returns: linker's rpath options
# Usage: 
# DLIBS := path/to/ABC/libABC.so
# DLIBS += path/to/QWE/libQWE.so
# LDFLAGS += $(call GET_DYNLIB_LDFLAGS_RPATH, $(DLIBS))  
## will be -Wl,-rpath,path/to/ABC -Wl,-rpath,path/to/QWE
#------------------------------------------------------------------------------
GET_DYNLIB_LDFLAGS_RPATH = $(addprefix -Wl$(COMMA)-rpath$(COMMA),$(dir $(call GET_DEPS,$(1)))) 

#------------------------------------------------------------------------------
# Formats linker options for group of libraries.
#
# Function: GROUP_LIBS
# Arguments: 1: linker ldflags with libraries
# Returns: linker ldflags with grouped libraries
# Usage: 
# LDFLAGS := -Lpath/to/ABC/libABC.a -lQWE
# LDFLAGS := $(call GROUP_LIBS, $(LDFLAGS))  
## will be -Wl,--start-group -Lpath/to/ABC/libABC.a -lQWE -Wl,--end-group
#------------------------------------------------------------------------------
GROUP_LIBS = -Wl,--start-group $(1) -Wl,--end-group

#------------------------------------------------------------------------------
# Returns ldflags for linking the list of dynamic and static libraries
#
# Function: GROUP_LIBS
# Arguments: 1: list of static libraries
#			 2: list of dynamic libraries
# Returns: linker ldflags with librariers link options
# Usage: 
# LIBS := path/to/ABC/libABC.a 
# DLIBS := path/to/QWE/libQWE.so
# LDFLAGS := $(call GET_LIB_LDFLAGS, $(LIBS), $(DLIBS))
## will be -Lpath/to/QWE -Wl,--start-group -Lpath/to/ABC/libABC.a -lQWE -Wl,--end-group -Wl,-rpath,path/to/QWE
#------------------------------------------------------------------------------
GET_LIB_LDFLAGS = $(call GET_DYNLIB_LDFLAGS_L,$(2)) $(call GROUP_LIBS,$(call GET_DEPS,$(1)) $(call GET_DYNLIB_LDFLAGS_l,$(2))) $(call GET_DYNLIB_LDFLAGS_RPATH,$(2))
#GET_LIB_LDFLAGS = $(call GET_DYNLIB_LDFLAGS_L,$(2)) $(call GROUP_LIBS,$(call GET_DEPS,$(1)) $(call GET_DYNLIB_LDFLAGS_l,$(2)))

