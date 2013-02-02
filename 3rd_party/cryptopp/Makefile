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


PKGNAME     :=cryptopp
PKGVER      :=5.6.1

SOURCE_DIR  :=$(PKGNAME)-$(PKGVER)
INSTALL_DIR :=$(3RD_PARTY_RES_DIR)

#LIB_NAME := libcryptopp.a
LIB_NAME := libcryptopp_rsa.a

###############################################################################
# Rules

debug: all

release: all

all: install

# Target will build package and create file 'build' to remember the time of build -
# this prevents the build of package every time we run make
build:  
	$(MAKE) -C $(SOURCE_DIR) $(LIB_NAME)
	touch build

$(3RD_PARTY_RES_DIR)lib/$(LIB_NAME): build
	cp $(SOURCE_DIR)/$(LIB_NAME) $(3RD_PARTY_RES_DIR)lib/$(LIB_NAME)
	#PREFIX=$(INSTALL_DIR) $(MAKE) -C $(SOURCE_DIR) install

install: $(3RD_PARTY_RES_DIR)lib/$(LIB_NAME)
	touch install

clean:

3rdp_clean:
	$(MAKE) -C $(SOURCE_DIR) clean
	-rm $(3RD_PARTY_RES_DIR)lib/$(LIB_NAME)
	-rm install
	-rm build

.PHONY: all debug release download check_download clean 3rdp_clean install
