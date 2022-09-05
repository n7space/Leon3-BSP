include definitions.mk

TEST_DIR = test
SRC_DIR = src
SIS_DIR = sis
BUILD_DIR = build

export TEST = test.exe

all: $(BUILD_DIR)/libUART.a $(BUILD_DIR)/$(SIS_NAME)-$(SIS_VERSION)

$(BUILD_DIR)/libUART.a:
	$(MAKE) -C $(SRC_DIR) libUART.a

$(BUILD_DIR)/$(TEST):
	$(MAKE) -C $(TEST_DIR) test

$(BUILD_DIR)/$(SIS_NAME)-$(SIS_VERSION):
	$(MAKE) -C $(SIS_DIR) sis

test: $(BUILD_DIR)/libUART.a $(BUILD_DIR)/$(SIS_NAME)-$(SIS_VERSION) $(BUILD_DIR)/$(TEST)
	$(SIS_DIR)/$(BUILD_DIR)/$(SRC_DIR)/$(SIS_NAME)-$(SIS_VERSION) -leon3 -d 10 -freq 100 -m 4 -r -v -uart1 uart $(BUILD_DIR)/$(TEST)

gdb: $(BUILD_DIR)/libUART.a $(BUILD_DIR)/$(SIS_NAME)-$(SIS_VERSION) $(BUILD_DIR)/$(TEST)
	$(BUILD_DIR)/$(SIS_NAME)-$(SIS_VERSION) -leon3 -gdb -port 1234 -d 10 -freq 100 -m 4 -r -v $(BUILD_DIR)/$(TEST)

sis: $(BUILD_DIR)/$(SIS_NAME)-$(SIS_VERSION)

clean:
	$(MAKE) -C $(SRC_DIR) clean
	$(MAKE) -C $(TEST_DIR) clean
	rm -rf $(BUILD_DIR)

.PHONY: sis test gdb clean

.DEFAULT_GOAL := all
