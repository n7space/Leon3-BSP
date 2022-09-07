include ../../definitions.mk

all: uart_integration_test

uart_integration_test:
	$(MAKE) -C $(UART_SRC_DIR) check
	$(MAKE) -C $(UART_SRC_DIR) clean

clean:
	$(MAKE) -C $(UART_SRC_DIR) clean
	rm -rf $(INTEGRATION_TEST_DIR)

.PHONY: clean

.DEFAULT_GOAL := all