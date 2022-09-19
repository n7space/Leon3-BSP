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
TIMER_SRC_DIR = Timer
UART_SRC_DIR = Uart
UTILS_SRC_DIR = Utils
SYSTEM_CONFIG_SRC_DIR = SystemConfig

HOST_CXX = g++
HOST_CC = gcc
HOST_AR = ar

SPARC_AR = $(RTEMS_CPU)-rtems$(RTEMS_API)-ar
SPARC_AS = $(RTEMS_CPU)-rtems$(RTEMS_API)-as
SPARC_CC = $(RTEMS_CPU)-rtems$(RTEMS_API)-gcc
SPARC_CXX = $(RTEMS_CPU)-rtems$(RTEMS_API)-g++
SPARC_LD = $(RTEMS_CPU)-rtems$(RTEMS_API)-ld
SPARC_NM = $(RTEMS_CPU)-rtems$(RTEMS_API)-nm
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

DEFFLAGS = -DMOCK_REGISTERS
