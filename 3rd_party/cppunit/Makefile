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


PKGNAME     :=cppunit
PKGVER      :=1.12.1

SOURCE_DIR  :=$(PKGNAME)/$(PKGNAME)
INSTALL_DIR :=$(3RD_PARTY_DIR)

###############################################################################
# Rules

debug: all

release: all

all: install

$(SOURCE_DIR)/Makefile:
	cd $(SOURCE_DIR); ./configure LDFLAGS='-ldl' --prefix=$(INSTALL_DIR) --disable-shared --enable-static

# Target will build package and create file 'build' to remember the time of build -
# this prevents the build of package every time we run make
build: $(SOURCE_DIR)/Makefile 
	$(MAKE) -C $(SOURCE_DIR)
	touch build

$(3RD_PARTY_DIR)/lib/libcppunit.a: build
	$(MAKE) -C $(SOURCE_DIR) install

# Target will install package and create file 'install' to remember the time of install -
# this prevents the isntall of package every time we run make
install: $(3RD_PARTY_DIR)/lib/libcppunit.a
	touch install

clean:

3rdp_clean:
	$(MAKE) -C $(SOURCE_DIR) clean distclean
	-rm install
	-rm build

.PHONY: all debug release download check_download clean 3rdp_clean
