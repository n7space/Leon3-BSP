include ../../definitions.mk

all: check uart_check

check:

uart_check:
	$(MAKE) -C $(UART_TEST_DIR) check

.PHONY: clean

.DEFAULT_GOAL := all