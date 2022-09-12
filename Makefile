include definitions.mk

all: sis_module uart uart_unit_test uart_integration_test

sis_module: 
	$(MAKE) -C $(SIS_MODULE_SRC_DIR) sis

uart:
	$(MAKE) -C $(SRC_DIR) uart

uart_unit_test:
	$(MAKE) -C $(TEST_DIR) uart_unit_test

uart_integration_test: sis_module uart 
	$(MAKE) -C $(TEST_DIR) uart_integration_test

clean:
	$(MAKE) -C $(SIS_MODULE_SRC_DIR) clean
	$(MAKE) -C $(TEST_DIR) clean
	$(MAKE) -C $(SRC_DIR) clean
	rm -rf $(BUILD_DIR)

.PHONY: clean

.DEFAULT_GOAL := all