include ../definitions.mk

SRC_DIR = src
BUILD_DIR=../build

SRC = $(wildcard ./**/*.c)
INCL = ./ $(sort $(dir $(wildcard ./**/*.h)))
PROJ_OBJS = $(addprefix $(BUILD_DIR)/$(SRC_DIR)/,$(patsubst %.c,%.o,$(SRC)))

CFLAGS = -g $(DEPFLAGS) $(WARNFLAGS) $(ABI_FLAGS) $(OPTFLAGS) $(addprefix -I,$(INCL)) -DRTEMS_API_$(RTEMS_API) -DRTEMS_SIS
CCLINK = $(CC) $(CFLAGS) -Wl,-Map,$(basename $@).map

all: $(BUILD_DIR)/libUART.a

$(BUILD_DIR):
	mkdir -p $(addprefix $(BUILD_DIR)/$(SRC_DIR)/,$(sort $(dir $(wildcard ./*/*))))

libUART.a: $(BUILD_DIR) $(PROJ_OBJS)
	$(AR) -crsv $(BUILD_DIR)/$@ $(PROJ_OBJS)

$(BUILD_DIR)/$(SRC_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/libUART.a $(PROJ_OBJS)

.PHONY: clean

.DEFAULT_GOAL := all
