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


PKGNAME :=log4cplus
PKGVER :=1.0.4

SOURCE_DIR :=$(PKGNAME)-$(PKGVER)
INSTALL_DIR :=$(3RD_PARTY_RES_DIR)

LIB_NAME := liblog4cplus.a
LIB_SO_NAME := liblog4cplus.so

LIB_INST_PATH := $(3RD_PARTY_RES_DIR)lib/$(LIB_NAME)
LIB_SO_INST_PATH := $(3RD_PARTY_RES_DIR)lib/$(LIB_SO_NAME)

LIBS = $(LIB_INST_PATH) $(LIB_SO_INST_PATH)

###############################################################################
# Rules

debug: all

release: all

all: install

$(SOURCE_DIR)/Makefile:
	cd $(SOURCE_DIR); ./configure --prefix=$(INSTALL_DIR) --enable-static


prebuild:
	cd $(SOURCE_DIR); automake --add-missing; cd ..

# Target will build package and create file 'build' to remember the time of build -
# this prevents the build of package every time we run make
build: $(SOURCE_DIR)/Makefile
	$(MAKE) -C $(SOURCE_DIR)
	touch build

$(3RD_PARTY_RES_DIR)lib/liblog4cplus.a: build
	$(MAKE) -C $(SOURCE_DIR) install

# Target will install package and create file 'install' to remember the time of install -
# this prevents the isntall of package every time we run make
install: prebuild $(LIBS)
	touch install

clean:

3rdp_clean:
	$(MAKE) -C $(SOURCE_DIR) clean distclean
		-rm install
		-rm build

.PHONY: all debug release download check_download clean 3rdp_clean prebuild