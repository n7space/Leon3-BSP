include ../definitions.mk

SRC_DIR = ../src
BUILD_DIR = ../build
TEST_DIR = test
TEST := test.exe

FUNCTION_TESTS_DIR = function_tests
FUNCTION_TEST_SRC = $(wildcard $(FUNCTION_TESTS_DIR)/**/*.c)
FUNCTION_TESTS = $(addprefix $(BUILD_DIR)/$(TEST_DIR)/,$(patsubst %.c,%.o,$(FUNCTION_TEST_SRC)))

INCL = $(SRC_DIR)/ $(sort $(dir $(wildcard $(SRC_DIR)/**/*.h))) $(sort $(dir $(wildcard $(FUNCTION_TESTS_DIR)/**/*.h)))
STATIC_LIBS = -Wl,-Bstatic $(BUILD_DIR)/libUART.a

CFLAGS = -g $(OPTFLAGS) $(shell pkg-config --cflags $(PKG_CONFIG)) $(addprefix -I,$(INCL))
CCLINK = $(CC) $(CFLAGS) -Wl,-Map,$(basename $@).map

all: test

test: $(BUILD_DIR)/$(TEST)

$(BUILD_DIR)/libUART.a:
	$(MAKE) -C $(SRC_DIR) libUART.a

$(BUILD_DIR)/$(TEST_DIR):
	mkdir -p $(addprefix $(BUILD_DIR)/$(TEST_DIR)/,$(sort $(dir $(wildcard $(FUNCTION_TESTS_DIR)/**/*.h))))

$(BUILD_DIR)/$(TEST_DIR)/main.o : main.c | $(BUILD_DIR)/$(TEST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TEST_DIR)/$(FUNCTION_TESTS_DIR)/%.o : $(FUNCTION_TESTS_DIR)/%.c | $(BUILD_DIR)/$(TEST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TEST): $(BUILD_DIR)/libUART.a $(FUNCTION_TESTS) $(BUILD_DIR)/$(TEST_DIR)/main.o
	$(CCLINK) $(PROJ_OBJS) $(FUNCTION_TESTS) $(BUILD_DIR)/$(TEST_DIR)/main.o $(STATIC_LIBS) $(LDFLAGS) $(addprefix -I,$(TEST_INCL)) -o $@

clean:
	rm -f $(FUNCTION_TESTS) $(BUILD_DIR)/$(TEST_DIR)/main.o $(BUILD_DIR)/$(TEST)

.PHONY: clean

.DEFAULT_GOAL := all
