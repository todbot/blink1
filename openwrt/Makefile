#
# Ipkg Makefile for building blink1-tool on OpenWrt / Arduino Yun
#
# Symlink this directory into your openwrt 'package' directory,
# with name 'blink1', then add it as a package, e.g. 
# % cd ~/projects/linino/trunk/package
# % ln -s ~/projects/blink1/openwrt blink1
# % cd ~/projects/linino/trunk
# % ./scripts/feeds install blink1
# % make menuconfig  (select 'blink1' in Utilities, then save new config)
# % make V=s package/blink1/compile
# % scp bin/ar71xx/packages/blink1_007-1_ar71xx.ipk root@arduino.local:
# # opkg install blink1_007_ar71xx.ipk (on yun or equiv)
#
#
include $(TOPDIR)/rules.mk

PKG_NAME:=blink1
PKG_VERSION:=007
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
#PKG_SOURCE_URL:=http://todbot.com/blink1/tst
#PKG_MD5SUM:=a76c271ebeaa3eebbea793b71a9175d8
#PKG_BUILD_PARALLEL:=1
#PKG_INSTALL:=1

PKG_MAINTAINER := Tod Kurt <tod@thingm.com>

include $(INCLUDE_DIR)/package.mk

define Package/blink1
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb-compat 
  TITLE:=blink(1) device utilities
  URL:=http://blink1.thingm.com/
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	 $(MAKE) USBLIB_TYPE=HIDDATA OS="wrt" blink1-tool -C $(PKG_BUILD_DIR) \
                $(TARGET_CONFIGURE_OPTS) \
                COPT_FLAGS="$(TARGET_CFLAGS)"  \
                LDOPT_FLAGS="$(TARGET_LDFLAGS) "
endef

define Package/blink1/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/blink1-tool $(1)/usr/bin/
endef

$(eval $(call BuildPackage,blink1))
