BUILD_DIR=../build
SRC = $(wildcard ./**/*.c)
INCL = ./ $(sort $(dir $(wildcard ./**/*.h)))
PROJ_OBJS = $(addprefix $(BUILD_DIR)/src/,$(patsubst %.c,%.o,$(SRC)))

DEPFLAGS = -MT $@ -MD -MP -MF $(basename $@).d
WARNFLAGS = -Wall -Wextra
OPTFLAGS = -Os -ffunction-sections -fdata-sections
ABI_FLAGS = $(shell pkg-config --cflags $(PKG_CONFIG))

LDFLAGS = $(shell pkg-config --libs $(PKG_CONFIG))
CFLAGS = $(DEPFLAGS) $(WARNFLAGS) $(ABI_FLAGS) $(OPTFLAGS) $(addprefix -I,$(INCL)) -DRTEMS_API_$(RTEMS_API) -DRTEMS_SIS

all: $(PROJ_OBJS)

$(BUILD_DIR)/$(SRC_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

.DEFAULT_GOAL := all
