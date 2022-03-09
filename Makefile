RTEMS_API = 6
RTEMS_CPU = sparc
RTEMS_BSP = gr712rc-qual-only

RTEMS_ROOT = /opt/rtems-6-sparc-gr712rc-smp-3
PKG_CONFIG = $(RTEMS_ROOT)/lib/pkgconfig/$(RTEMS_CPU)-rtems$(RTEMS_API)-$(RTEMS_BSP).pc
BUILDDIR = build

SIS_DIR=sis

SRC = $(wildcard src/**/*.c)
INCL = src/ $(sort $(dir $(wildcard src/**/*.h)))

DEPFLAGS = -MT $@ -MD -MP -MF $(basename $@).d
WARNFLAGS = -Wall -Wextra
OPTFLAGS = -g -O0 -ffunction-sections -fdata-sections
ABI_FLAGS = $(shell pkg-config --cflags $(PKG_CONFIG))
EXEEXT = .exe

LDFLAGS = $(shell pkg-config --libs $(PKG_CONFIG))
CFLAGS = $(DEPFLAGS) $(WARNFLAGS) $(ABI_FLAGS) $(OPTFLAGS) $(addprefix -I,$(INCL)) -DRTEMS_API_$(RTEMS_API) -DRTEMS_SIS

CCLINK = $(CC) $(CFLAGS) -Wl,-Map,$(basename $@).map

export PATH := $(RTEMS_ROOT)/bin:$(PATH)
export AR = $(RTEMS_ROOT)/sparc-rtems6/bin/ar
export AS = $(RTEMS_ROOT)/sparc-rtems6/bin/as
export CC = $(RTEMS_CPU)-rtems$(RTEMS_API)-gcc
export LD = $(RTEMS_ROOT)/sparc-rtems6/bin/ld
export NM = $(RTEMS_ROOT)/sparc-rtems6/bin/nm
export OBJCOPY = $(RTEMS_ROOT)/sparc-rtems6/bin/objcopy
export RANLIB = $(RTEMS_ROOT)/sparc-rtems6/bin/ranlib
export SIZE = $(RTEMS_CPU)-rtems$(RTEMS_API)-size
export STRIP = $(RTEMS_ROOT)/sparc-rtems6/bin/strip

TEST = $(BUILDDIR)/test
PROJ_OBJS = $(addprefix $(BUILDDIR)/,$(patsubst %.c,%.o,$(SRC)))

all: $(TEST)$(EXEEXT)

$(BUILDDIR):
	mkdir -p $(addprefix $(BUILDDIR)/,$(sort $(dir $(wildcard src/**/*))))

$(TEST)$(EXEEXT): $(BUILDDIR) $(PROJ_OBJS)
	$(CCLINK) $(PROJ_OBJS) $(LDFLAGS) -o $@
	$(MAKE) -C $(SIS_DIR) all

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TEST)$(EXEEXT)
	$(MAKE) -C $(SIS_DIR) all
	$(SIS_DIR)/sis -leon3 -d 10 -freq 100 -m 4 -r -v $<

gdb: $(TEST)$(EXEEXT)
	$(MAKE) -C $(SIS_DIR) all
	$(SIS_DIR)/sis -leon3 -gdb -port 1234 -d 10 -freq 100 -m 4 -r -v $<

clean:
	$(MAKE) -C $(SIS_DIR) clean
	rm -rf $(BUILDDIR)
