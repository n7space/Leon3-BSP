include ../definitions.mk

all: test

tests:
	$(MAKE) -C $(UNIT_TEST_DIR) test

check:
	$(MAKE) -C $(INTEGRATION_TEST_DIR) check

# uart_unit_check:
# 	$(MAKE) -C $(UNIT_TEST_DIR) uart_check

# unit_check:
# 	$(MAKE) -C $(UNIT_TEST_DIR) check

# uart_integration_check:
# 	$(MAKE) -C $(INTEGRATION_TEST_DIR) uart_check

# integration_check:
# 	$(MAKE) -C $(INTEGRATION_TEST_DIR) check

clean:
	$(MAKE) -C $(UNIT_TEST_DIR) clean
	$(MAKE) -C $(INTEGRATION_TEST_DIR) clean

.PHONY: clean

.DEFAULT_GOAL := all