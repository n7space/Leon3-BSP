include ../definitions.mk

all: uart

timer:
	$(MAKE) -C $(TIMER_SRC_DIR) libtimer

uart: 
	$(MAKE) -C $(UART_SRC_DIR) libuart

clean:
	$(MAKE) -C $(UART_SRC_DIR) clean
	rm -rf $(SRC_BUILD_DIR)

.PHONY: clean

.DEFAULT_GOAL := all