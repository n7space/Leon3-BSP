include ../definitions.mk

all: uart_unit_test uart_integration_test

uart_integration_test:
	$(MAKE) -C $(INTEGRATION_TEST_DIR) uart_integration_test

uart_unit_test:
	$(MAKE) -C $(UNIT_TEST_DIR) uart_unit_test

clean:
	$(MAKE) -C $(INTEGRATION_TEST_DIR) clean
	rm -rf $(TEST_DIR)

.PHONY: clean

.DEFAULT_GOAL := all