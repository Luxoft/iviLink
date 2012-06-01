#$Id: common.mk 891 2011-11-30 12:09:49Z dlobashevskiy $

#------------------------------------------------------------------------------
# First rule - will be default
#------------------------------------------------------------------------------
debug:


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



