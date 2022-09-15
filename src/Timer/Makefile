
include ../../definitions.mk

TIMER_LIB_BUILD_DIR = ../../$(BUILD_DIR)/$(SRC_DIR)/$(TIMER_SRC_DIR)

CFLAGS = -g $(DEPFLAGS) $(WARNFLAGS) $(ABI_FLAGS) $(OPTFLAGS) -DRTEMS_API_$(RTEMS_API) -DRTEMS_SIS

SRC = $(wildcard ./*.c)
INCL = $(addprefix -I,$(sort $(dir $(wildcard ./*.h) $(wildcard ./../Utils/*.h) $(wildcard ./../SystemConfig/*.h))))
OBJECTS = $(patsubst %.c,$(TIMER_LIB_BUILD_DIR)/%.o, $(SRC))

all: libtimer

libtimer: $(OBJECTS)
	$(AR) -crsv $(TIMER_LIB_BUILD_DIR)/$@.a $(OBJECTS)

$(TIMER_LIB_BUILD_DIR):
	mkdir -p $(TIMER_LIB_BUILD_DIR)

$(TIMER_LIB_BUILD_DIR)/%.o: %.c | $(TIMER_LIB_BUILD_DIR)
	$(CC) $(CONFIG) $(DEFS) $(INCL) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJECTS) $(TIMER_LIB_BUILD_DIR)

.PHONY: clean

.DEFAULT_GOAL := libtimer