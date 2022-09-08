ROOT_DIR = ../..

include $(ROOT_DIR)/definitions.mk

CPPUTEST_HOME = /opt/cpputest
CPPUTEST_INCL = -I$(CPPUTEST_HOME)/include
CPPUTEST_LIB = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

LIBUART = ../../$(BUILD_DIR)/$(SRC_DIR)/$(UART_SRC_DIR)/libuart.a

MOCK_HEADERS_DIR = mock_headers

TESTS_BUILD_DIR = $(ROOT_DIR)/$(BUILD_DIR)/$(UNIT_TEST_DIR)

SRC = main.cc  $(wildcard ./**/*.cc)
INCL = $(addprefix -I,$(sort $(dir $(wildcard ./$(ROOT_DIR)/$(SRC_DIR)/**/*.h)) $(MOCK_HEADERS_DIR)))
STATIC_LIBS = -Bstatic $(LIBUART)

OBJECTS := $(patsubst %.cc,$(TESTS_BUILD_DIR)/%.o, $(SRC))

CCLINK = $(G++) -Wl,-Map,$(TESTS_BUILD_DIR)/$(basename $@).map

all: test

test: $(OBJECTS) 
	$(CCLINK) $(OBJECTS) $(STATIC_LIBS) $(CPPUTEST_LIB) -o $(TESTS_BUILD_DIR)/$@

$(TESTS_BUILD_DIR):
	mkdir -p $(addprefix $(TESTS_BUILD_DIR)/,$(sort $(dir $(SRC))))

$(TESTS_BUILD_DIR)/%.o: %.cc | $(TESTS_BUILD_DIR)
	$(G++) $(INCL) $(CPPUTEST_INCL) -c -o $@ $<

check: test
	./$(TESTS_BUILD_DIR)/test -c

uart_check: test
	./$(TESTS_BUILD_DIR)/test -c -g UartTests


clean:
	rm -rf $(TESTS) $(TESTS_BUILD_DIR)
.PHONY: clean

.DEFAULT_GOAL := all
