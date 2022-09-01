include definitions.mk

all: sis_module timers

sis_module: 
	$(MAKE) -C $(SIS_MODULE_SRC_DIR) sis

timers:
	$(MAKE) -C $(SRC_DIR) timers

tests: timers
	$(MAKE) -C $(TEST_DIR) tests
	
timers_unit_check: tests
	$(MAKE) -C $(TEST_DIR) timers_unit_check

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