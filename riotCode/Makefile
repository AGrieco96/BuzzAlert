APPLICATION = black_app

BOARD ?= nucleo-f401re

RIOTBASE ?= $(CURDIR)/../../RIOT
#RIOTBASE ?= $(CURDIR)/../../RIOT-2022.04-branch

# Provide network connectivity via the USB

# Default to using ethos for providing the uplink when not on native
UPLINK ?= ethos

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_default
# Optimize the use of network interface
USEMODULE += gnrc_netif_single  #non necessario
USEMODULE += stdio_ethos gnrc_uhcpc
# Include MQTT-SN client
USEMODULE += emcute
# Include servo Driver
USEMODULE += servo

EMCUTE_ID ?= 01
ETHOS_BAUDRATE ?= 115200
TAP ?= tap0
USE_DHCPV6 ?= 0
# IPV6_PREFIX ?= fec0:affe::1/64
IPV6_PREFIX ?= fe80:2::/64
# IPv6 and port of the MQTT broker
SERVER_ADDR = fec0:affe::1
SERVER_PORT = 1885


CFLAGS += -DSERVER_ADDR='"$(SERVER_ADDR)"'
CFLAGS += -DSERVER_PORT='$(SERVER_PORT)'

#Allow for env-var-based override of the nodes name (EMCUTE_ID)
ifneq (,$(EMCUTE_ID))
  CFLAGS += -DEMCUTE_ID=\"$(EMCUTE_ID)\"
endif

#Modules to include

USEMODULE += periph_gpio
USEMODULE += xtimer
USEMODULE += periph_gpio_irq
USEMODULE += analog_util

FEATURES_REQUIRED = periph_gpio
FEATURES_REQUIRED = periph_adc

include $(CURDIR)/Makefile.ethos.conf
include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)
