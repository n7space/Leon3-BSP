RTEMS_API = 6
RTEMS_CPU = sparc
RTEMS_BSP = gr712rc-qual-only
RTEMS_ROOT = /opt/rtems-6-sparc-gr712rc-smp-4

SIS_NAME = sis
SIS_VERSION = 2.29

TSIM = tsim-leon3

AR = $(RTEMS_ROOT)/sparc-rtems6/bin/ar
AS = $(RTEMS_ROOT)/sparc-rtems6/bin/as
CC = $(RTEMS_CPU)-rtems$(RTEMS_API)-gcc
LD = $(RTEMS_ROOT)/sparc-rtems6/bin/ld
NM = $(RTEMS_ROOT)/sparc-rtems6/bin/nm
OBJCOPY = $(RTEMS_ROOT)/sparc-rtems6/bin/objcopy
RANLIB = $(RTEMS_ROOT)/sparc-rtems6/bin/ranlib
SIZE = $(RTEMS_CPU)-rtems$(RTEMS_API)-size
STRIP = $(RTEMS_ROOT)/sparc-rtems6/bin/strip

PKG_CONFIG := $(RTEMS_ROOT)/lib/pkgconfig/$(RTEMS_CPU)-rtems$(RTEMS_API)-$(RTEMS_BSP).pc

DEPFLAGS = -MT $@ -MD -MP -MF $(basename $@).d
WARNFLAGS = -Wall -Wextra
OPTFLAGS = -O0
ABI_FLAGS = $(shell pkg-config --cflags $(PKG_CONFIG))
LDFLAGS = $(shell pkg-config --libs $(PKG_CONFIG))
