include ../definitions.mk

all: test

uart_integration_test:
	$(MAKE) -C $(INTEGRATION_TEST_DIR) uart_integration_test

clean:
	$(MAKE) -C $(INTEGRATION_TEST_DIR) clean
	rm -rf $(TEST_DIR)

.PHONY: clean

.DEFAULT_GOAL := all