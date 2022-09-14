include ../../definitions.mk

UART_LIB_BUILD_DIR = ../../$(BUILD_DIR)/$(SRC_DIR)/$(UART_SRC_DIR)

CFLAGS = -g $(DEPFLAGS) $(WARNFLAGS) $(ABI_FLAGS) $(OPTFLAGS) -DRTEMS_API_$(RTEMS_API) -DRTEMS_SIS

SRC = $(wildcard ./*.c)
INCL = $(addprefix -I,$(sort $(dir $(wildcard ./*.h) $(wildcard ./../Utils/*.h) $(wildcard ./../SystemConfig/*.h))))
OBJECTS = $(patsubst %.c,$(UART_LIB_BUILD_DIR)/%.o, $(SRC))

all: libuart

libuart: $(OBJECTS)
	$(AR) -crsv $(UART_LIB_BUILD_DIR)/$@.a $(OBJECTS)

$(UART_LIB_BUILD_DIR):
	mkdir -p $(UART_LIB_BUILD_DIR)

$(UART_LIB_BUILD_DIR)/%.o: %.c | $(UART_LIB_BUILD_DIR)
	$(CC) $(CONFIG) $(DEFS) $(INCL) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJECTS) $(UART_LIB_BUILD_DIR)

.PHONY: clean

.DEFAULT_GOAL := libuart