RTEMS_API = 6
RTEMS_CPU = sparc
RTEMS_BSP = gr712rc-qual-only
RTEMS_ROOT = /opt/rtems-6-sparc-gr712rc-smp-3

export SIS_NAME = sis
export SIS_VERSION = 2.29

export PKG_CONFIG = $(RTEMS_ROOT)/lib/pkgconfig/$(RTEMS_CPU)-rtems$(RTEMS_API)-$(RTEMS_BSP).pc
export SIS_DIR = sis
export SRC_DIR = src
export BUILD_DIR = build

SRC = $(wildcard $(SRC_DIR)/**/*.c)
INCL = $(SRC_DIR)/ $(sort $(dir $(wildcard $(SRC_DIR)/**/*.h)))
PROJ_OBJS = $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(SRC)))

DEPFLAGS = -MT $@ -MD -MP -MF $(basename $@).d
WARNFLAGS = -Wall -Wextra
OPTFLAGS = -Os -ffunction-sections -fdata-sections
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

DEMO = $(BUILD_DIR)/demo
DEMO_DIR = demo/
DEMO_OBJ = $(DEMO_DIR)/main.o

TEST = $(BUILD_DIR)/test
TEST_DIR = test/
TESTS = $(TEST_DIR)/main.c $(wildcard $(TEST_DIR)/**/*.c)
TEST_OBJS = $(patsubst %.c,%.o,$(TESTS))

all: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/$(SIS_DIR)
	$(MAKE) -C $(SIS_DIR) all
	mkdir -p $(addprefix $(BUILD_DIR)/,$(sort $(dir $(wildcard $(SRC_DIR)/**/*))))
	$(MAKE) -C $(SRC_DIR) all

$(DEMO)$(EXEEXT): $(BUILD_DIR)
	$(MAKE) -C $(DEMO_DIR)
	$(CCLINK) $(PROJ_OBJS) $(DEMO_OBJ) $(LDFLAGS) -o $@

$(TEST)$(EXEEXT): $(BUILD_DIR)
	$(MAKE) -C $(TEST_DIR) all
	$(CCLINK) $(PROJ_OBJS) $(TEST_OBJS) $(LDFLAGS) -I$(TEST_DIR) -o $@

test: $(TEST)$(EXEEXT)
	$(BUILD_DIR)/$(SIS_NAME)-$(SIS_VERSION) -leon3 -d 10 -freq 100 -m 4 -r -v $<

gdb: $(TEST)$(EXEEXT)
	$(BUILD_DIR)/$(SIS_NAME)-$(SIS_VERSION) -leon3 -gdb -port 1234 -d 10 -freq 100 -m 4 -r -v $<

demo: $(DEMO)$(EXEEXT)
	$(BUILD_DIR)/$(SIS_NAME)-$(SIS_VERSION) -leon3 -d 10 -freq 100 -m 4 -r -v $<

clean:
	$(MAKE) -C $(SRC_DIR) clean
	$(MAKE) -C $(TEST_DIR) clean
	$(MAKE) -C $(DEMO_DIR) clean
	rm -rf $(BUILD_DIR)

.PHONY: test gdb demo clean

.DEFAULT_GOAL := all
