include ../definitions.mk

SRC_BUILD_DIR = ../$(BUILD_DIR)/$(SRC_DIR)

all: timers

timers: 
	$(MAKE) -C $(TIMER_SRC_DIR) libtimer

clean:
	$(MAKE) -C $(TIMER_SRC_DIR) clean
	rm -rf $(SRC_BUILD_DIR)

.PHONY: clean

.DEFAULT_GOAL := all
