# 
# iviLINK SDK, version 1.1.2
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

OUT_PATH := $(call OUT_PATH_FUNC)
$(info OUT_PATH $(OUT_PATH))

OBJS_PATH := $(OUT_PATH)obj

SEAT_PROTO   := timestamp.proto climate-state.proto
SEAT_HEADERS := $(SEAT_PROTO:.proto=.pb.h)
SEAT_OBJS    := $(SEAT_PROTO:.proto=.pb.o)

PROTO_DIR ?= .
GENDIR ?= ./generated

INCLUDES += -I$(GENDIR)
CFLAGS +=-fPIC

SEAT_OBJS_FULL_PATH := $(addprefix $(OBJS_PATH)/,$(notdir $(SEAT_OBJS)))
SEAT_TARGETS := $(SEAT_OBJS_FULL_PATH)
SEAT_HEADERS_FULL_PATH := $(addprefix $(GENDIR)/,$(notdir $(SEAT_HEADERS)))
SEAT_TARGETS += $(SEAT_HEADERS_FULL_PATH)

OBJS += $(SEAT_OBJS_FULL_PATH)
OUT_FNAME	:= libClimateState.so

SEAT_LIB	:= $(OUT_PATH)$(OUT_FNAME)
###############################################################################
# protobuf

maybe-mkdir = $(if $(wildcard $1),,mkdir -p $1)

PROTOC := protoc

# Make a directory unless it already exists
maybe-mkdir = $(if $(wildcard $1),,mkdir -p $1)

# C++ build rule.
$(OBJS_PATH)/%.o: $(GENDIR)/%.cc
	$(call maybe-mkdir, $(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

# Protobuffer code generation rules (note the first rule has multiple targets).
$(GENDIR)/%.pb.h $(GENDIR)/%.pb.cc: $(PROTO_DIR)/%.proto
	$(call maybe-mkdir, $(dir $@))
	$(PROTOC) -I$(PROTO_DIR) --cpp_out=$(GENDIR) $<

$(OBJS_PATH)/%.pb.o: $(GENDIR)/%.pb.cc
	$(CC) $(PROTO_CFLAGS) -c $< -o $@

$(SEAT_LIB): $(SEAT_HEADERS_FULL_PATH) $(SEAT_TARGETS)
	$(CC) $(SEAT_OBJS_FULL_PATH) -shared -o $(SEAT_LIB)

seat-proto:	$(SEAT_LIB)

seat-proto-clean:
	-rm $(SEAT_TARGETS)
	rm -rf $(GENDIR)

debug:  seat-proto
release: seat-proto
all:	seat-proto
clean:	seat-proto-clean

.PHONY: seat-proto seat-proto-clean debug release all clean
