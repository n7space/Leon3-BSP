include definitions.mk

all: sis_module uart

sis_module: 
	$(MAKE) -C $(SIS_MODULE_SRC_DIR) sis

uart:
	$(MAKE) -C $(SRC_DIR) uart

tests: uart
	$(MAKE) -C $(TEST_DIR) tests
	
uart_unit_check: tests
	$(MAKE) -C $(TEST_DIR) uart_unit_check

unit_check: tests
	$(MAKE) -C $(TEST_DIR) unit_check

integration_check:

check: unit_check integration_check

clean:
	$(MAKE) -C $(SIS_MODULE_SRC_DIR) clean
	$(MAKE) -C $(SRC_DIR) clean
	rm -rf $(BUILD_DIR)

.PHONY: clean

.DEFAULT_GOAL := all