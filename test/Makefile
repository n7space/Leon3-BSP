include ../definitions.mk

all: timer_unit_test timer_integration_test uart_unit_test uart_integration_test

timer_unit_test:
	$(MAKE) -C $(UNIT_TEST_DIR) timer_unit_test

timer_integration_test:
	$(MAKE) -C $(INTEGRATION_TEST_DIR) timer_integration_test

uart_unit_test:
	$(MAKE) -C $(UNIT_TEST_DIR) uart_unit_test

uart_integration_test:
	$(MAKE) -C $(INTEGRATION_TEST_DIR) uart_integration_test

clean:
	$(MAKE) -C $(INTEGRATION_TEST_DIR) clean
	rm -rf $(TEST_DIR)

.PHONY: clean

.DEFAULT_GOAL := all
