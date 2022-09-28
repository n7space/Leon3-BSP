ROOT_DIR = ../..

include $(ROOT_DIR)/definitions.mk

CPPUTEST_HOME = /opt/cpputest
CPPUTEST_INCL = -I$(CPPUTEST_HOME)/include
CPPUTEST_LIB = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

UART_DIR = $(ROOT_DIR)/$(SRC_DIR)/$(UART_SRC_DIR)
TIMER_DIR = $(ROOT_DIR)/$(SRC_DIR)/$(TIMER_SRC_DIR)
TESTS_BUILD_DIR = $(ROOT_DIR)/$(BUILD_DIR)/$(UNIT_TEST_DIR)
UART_TEST_LIB_BUILD_DIR = $(TESTS_BUILD_DIR)/$(UART_SRC_DIR)
TIMER_TEST_LIB_BUILD_DIR = $(TESTS_BUILD_DIR)/$(TIMER_SRC_DIR)
RTEMS_MOCK_LIB_BUILD_DIR = $(TESTS_BUILD_DIR)/$(MOCK_DIR)

RTEMS_MOC_SRC_DIR = $(ROOT_DIR)/$(TEST_DIR)/$(UNIT_TEST_DIR)/$(MOCK_DIR)

LIBUART = $(UART_TEST_LIB_BUILD_DIR)/libuart.a
LIBTIMER = $(TIMER_TEST_LIB_BUILD_DIR)/libtimer.a
LIBRTEMS_MOCK = $(RTEMS_MOCK_LIB_BUILD_DIR)/librtems_mock.a

CFLAGS = -g -Wall -Wextra -Os -ffunction-sections -fdata-sections

SRC = main.cc  $(wildcard ./**/*.cc) $(wildcard ./**/*.c)
UART_SRC = $(wildcard ./$(UART_DIR)/*.c)
TIMER_SRC = $(wildcard ./$(TIMER_DIR)/*.c)
RTEMS_SRC = $(wildcard ./$(RTEMS_MOC_SRC_DIR)/*.c)
INCL = $(addprefix -I,$(sort $(dir $(wildcard ./$(ROOT_DIR)/$(SRC_DIR)/**/*.h) \
			 $(wildcard ./$(RTEMS_MOC_SRC_DIR)/*.h))))
UART_INCL = $(addprefix -I,$(sort $(dir $(wildcard ./$(UART_DIR)/*.h) \
						$(wildcard ./$(ROOT_DIR)/$(SRC_DIR)/Utils/*.h) \
						$(wildcard ./$(ROOT_DIR)/$(SRC_DIR)/SystemConfig/*.h) \
						$(wildcard ./$(RTEMS_MOC_SRC_DIR)/*.h))))
TIMER_INCL = $(addprefix -I,$(sort $(dir $(wildcard ./$(TIMER_DIR)/*.h) \
						$(wildcard ./$(ROOT_DIR)/$(SRC_DIR)/Utils/*.h) \
						$(wildcard ./$(ROOT_DIR)/$(SRC_DIR)/SystemConfig/*.h) \
						$(wildcard ./$(RTEMS_MOC_SRC_DIR)/*.h))))
RTEMS_INCL = $(addprefix -I,$(sort $(dir $(wildcard ./$(RTEMS_MOC_SRC_DIR)/*.h))))

STATIC_LIBS = -Bstatic $(LIBUART) $(LIBTIMER) $(LIBRTEMS_MOCK)

OBJECTS = $(patsubst %.cc,$(TESTS_BUILD_DIR)/%.o, $(SRC))
UART_OBJECTS = $(patsubst %.c,$(UART_TEST_LIB_BUILD_DIR)/%.o, $(UART_SRC))
TIMER_OBJECTS = $(patsubst %.c,$(TIMER_TEST_LIB_BUILD_DIR)/%.o, $(TIMER_SRC))
RTEMS_OBJECTS = $(patsubst %.c,$(RTEMS_MOCK_LIB_BUILD_DIR)/%.o, $(RTEMS_SRC))

CCLINK = $(HOST_CXX) -Wl,-Map,$(TESTS_BUILD_DIR)/$(basename $@).map

all: test

libuart: librtems_mock $(UART_OBJECTS)
	$(HOST_AR) -crsv $(UART_TEST_LIB_BUILD_DIR)/$@.a $(UART_TEST_LIB_BUILD_DIR)/$(notdir $(UART_OBJECTS))

libtimer: librtems_mock $(TIMER_OBJECTS)
	$(HOST_AR) -crsv $(TIMER_TEST_LIB_BUILD_DIR)/$@.a $(patsubst %,$(TIMER_TEST_LIB_BUILD_DIR)/%, $(notdir $(TIMER_OBJECTS)))

librtems_mock: $(RTEMS_OBJECTS)
	$(HOST_AR) -crsv $(RTEMS_MOCK_LIB_BUILD_DIR)/$@.a $(RTEMS_MOCK_LIB_BUILD_DIR)/$(notdir $(RTEMS_OBJECTS))

test: libuart libtimer $(OBJECTS)
	$(CCLINK)  $(OBJECTS) $(STATIC_LIBS) $(CPPUTEST_LIB) -o $(TESTS_BUILD_DIR)/$@

$(TESTS_BUILD_DIR):
	mkdir -p $(addprefix $(TESTS_BUILD_DIR)/,$(sort $(dir $(SRC))))

$(TESTS_BUILD_DIR)/%.o: %.cc | $(TESTS_BUILD_DIR)
	$(HOST_CXX) $(INCL) $(CPPUTEST_INCL) $(CFLAGS) -o $@ -c $<

$(UART_TEST_LIB_BUILD_DIR):
	mkdir -p $(UART_TEST_LIB_BUILD_DIR)

$(UART_TEST_LIB_BUILD_DIR)/%.o: %.c | $(UART_TEST_LIB_BUILD_DIR)
	$(HOST_CC) $(UART_INCL) $(CFLAGS) $(DEFFLAGS) -o $(UART_TEST_LIB_BUILD_DIR)/$(notdir $@) -c $<

$(TIMER_TEST_LIB_BUILD_DIR):
	mkdir -p $(TIMER_TEST_LIB_BUILD_DIR)

$(TIMER_TEST_LIB_BUILD_DIR)/%.o: %.c | $(TIMER_TEST_LIB_BUILD_DIR)
	$(HOST_CC) $(TIMER_INCL) $(CFLAGS) $(DEFFLAGS) -o $(TIMER_TEST_LIB_BUILD_DIR)/$(notdir $@) -c $<

$(RTEMS_MOCK_LIB_BUILD_DIR):
	mkdir -p $(RTEMS_MOCK_LIB_BUILD_DIR)

$(RTEMS_MOCK_LIB_BUILD_DIR)/%.o: %.c | $(RTEMS_MOCK_LIB_BUILD_DIR)
	$(HOST_CC) $(RTEMS_INCL) $(CFLAGS) -o $(RTEMS_MOCK_LIB_BUILD_DIR)/$(notdir $@) -c $<

uart_unit_test: test
	./$(TESTS_BUILD_DIR)/test -c -g UartTests -v

timer_unit_test: test
	./$(TESTS_BUILD_DIR)/test -c -g TimerTests -v

clean:
	rm -rf $(TESTS) $(TESTS_BUILD_DIR)
.PHONY: clean

.DEFAULT_GOAL := all
