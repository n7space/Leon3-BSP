include definitions.mk

all: sis_module timer uart

sis_module: 
	$(MAKE) -C $(SIS_MODULE_SRC_DIR) sis

timer:
	$(MAKE) -C $(SRC_DIR) timer

uart:
	$(MAKE) -C $(SRC_DIR) uart

timer_unit_test:
	$(MAKE) -C $(TEST_DIR) timer_unit_test

timer_integration_test: sis_module timer
	$(MAKE) -C $(TEST_DIR) timer_integration_test

timer_test: timer_unit_test timer_integration_test

uart_unit_test:
	$(MAKE) -C $(TEST_DIR) uart_unit_test

uart_integration_test: sis_module uart 
	$(MAKE) -C $(TEST_DIR) uart_integration_test

uart_test: uart_unit_test uart_integration_test

test: timer_test uart_test

clean:
	$(MAKE) -C $(SIS_MODULE_SRC_DIR) clean
	$(MAKE) -C $(TEST_DIR) clean
	$(MAKE) -C $(SRC_DIR) clean
	rm -rf $(BUILD_DIR)

.PHONY: clean

.DEFAULT_GOAL := all
