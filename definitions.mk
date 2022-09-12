RTEMS_API = 6
RTEMS_CPU = sparc
RTEMS_BSP = gr712rc-qual-only
RTEMS_ROOT = /opt/rtems-6-sparc-gr712rc-smp-4

SIS_NAME = sis
SIS_VERSION = 2.29

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = test
UNIT_TEST_DIR = unit
INTEGRATION_TEST_DIR = integration
MOCK_DIR = mock
SIS_MODULE_SRC_DIR = sis
UART_SRC_DIR = Uart
UTILS_SRC_DIR = Utils
SYSTEM_CONFIG_SRC_DIR = SystemConfig

G++ = g++
GCC = gcc
GAR = ar

AR = $(RTEMS_ROOT)/sparc-rtems6/bin/ar
AS = $(RTEMS_ROOT)/sparc-rtems6/bin/as
CC = $(RTEMS_CPU)-rtems$(RTEMS_API)-gcc
CXX = $(RTEMS_CPU)-rtems$(RTEMS_API)-g++
LD = $(RTEMS_ROOT)/sparc-rtems6/bin/ld
NM = $(RTEMS_ROOT)/sparc-rtems6/bin/nm
OBJCOPY = $(RTEMS_ROOT)/sparc-rtems6/bin/objcopy
RANLIB = $(RTEMS_ROOT)/sparc-rtems6/bin/ranlib
SIZE = $(RTEMS_CPU)-rtems$(RTEMS_API)-size
STRIP = $(RTEMS_ROOT)/sparc-rtems6/bin/strip

PKG_CONFIG := $(RTEMS_ROOT)/lib/pkgconfig/$(RTEMS_CPU)-rtems$(RTEMS_API)-$(RTEMS_BSP).pc

DEPFLAGS = -MT $@ -MD -MP -MF $(basename $@).d
WARNFLAGS = -Wall -Wextra
OPTFLAGS = -Os -ffunction-sections -fdata-sections
ABI_FLAGS = $(shell pkg-config --cflags $(PKG_CONFIG))
LDFLAGS = $(shell pkg-config --libs $(PKG_CONFIG))
