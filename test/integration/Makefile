include ../../definitions.mk

all: timer_integration_test uart_integration_test

timer_integration_test:
	$(MAKE) -C $(TIMER_SRC_DIR) check
	$(MAKE) -C $(TIMER_SRC_DIR) clean

uart_integration_test:
	$(MAKE) -C $(UART_SRC_DIR) check
	$(MAKE) -C $(UART_SRC_DIR) clean

clean:
	$(MAKE) -C $(TIMER_SRC_DIR) clean
	$(MAKE) -C $(UART_SRC_DIR) clean
	rm -rf $(INTEGRATION_TEST_DIR)

.PHONY: clean

.DEFAULT_GOAL := all