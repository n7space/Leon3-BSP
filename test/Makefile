BUILD_DIR = ../build
SRC_DIR = ../src
FUNCTION_TESTS_DIR = ./function_tests
FUNCTION_TEST_INC = $(sort $(dir $(wildcard $(FUNCTION_TESTS_DIR)/**/*.h)))
FUNCTION_TEST_SRC = main.c $(wildcard $(FUNCTION_TESTS_DIR)/**/*.c)
FUNCTION_TESTS = $(patsubst %.c,%.o,$(FUNCTION_TEST_SRC))

INCL = $(SRC_DIR)/ $(sort $(dir $(wildcard $(SRC_DIR)/**/*.h))) $(FUNCTION_TEST_INC)
CFLAGS = $(shell pkg-config --cflags $(PKG_CONFIG)) $(addprefix -I,$(INCL))

all: $(FUNCTION_TESTS)

$(FUNCTION_TESTS_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -I. -I$(FUNCTION_TEST_INC) -c $< -o $@

clean:
	rm -f *.o $(FUNCTION_TESTS)

.PHONY: clean

.DEFAULT_GOAL := all
