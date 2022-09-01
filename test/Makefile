include ../definitions.mk

all: test

tests:
	$(MAKE) -C $(UNIT_TEST_DIR) test

timers_unit_check:
	$(MAKE) -C $(UNIT_TEST_DIR) timers_check

unit_check:
	$(MAKE) -C $(UNIT_TEST_DIR) check

clean:
	$(MAKE) -C $(UNIT_TEST_DIR) clean

.PHONY: clean

.DEFAULT_GOAL := all