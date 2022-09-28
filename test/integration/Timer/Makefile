ROOT_PATH = ../../..

include $(ROOT_PATH)/definitions.mk

TEST := test.exe

OUTPUT_FILE = system.output

CFLAGS = -g -O2 $(shell pkg-config --cflags $(PKG_CONFIG))
CCLINK = $(SPARC_CC) $(CFLAGS) -Wl,-Map,$(BUILD_DIR)/$(basename $@).map

INCL = $(addprefix -I,$(sort $(dir $(wildcard ./$(ROOT_PATH)/$(SRC_DIR)/$(TIMER_SRC_DIR)/*.h) \
       $(wildcard ./$(ROOT_PATH)/$(SRC_DIR)/$(UTILS_SRC_DIR)/*.h)  \
       $(wildcard ./$(ROOT_PATH)/$(SRC_DIR)/$(SYSTEM_CONFIG_SRC_DIR)/*.h))))

STATIC_LIBS = -Wl,-Bstatic $(ROOT_PATH)/$(BUILD_DIR)/$(SRC_DIR)/$(TIMER_SRC_DIR)/libtimer.a

TIMER_POLLING = $(patsubst %.c,$(BUILD_DIR)/%.o, timer_polling.c)
TIMER_INTERRUPT = $(patsubst %.c,$(BUILD_DIR)/%.o, timer_interrupt.c)

SIS_BINARY = $(ROOT_PATH)/$(SIS_MODULE_SRC_DIR)/$(BUILD_DIR)/$(SRC_DIR)/$(SIS_NAME)-$(SIS_VERSION)
SIS_PARAMETERS = -leon3 -d 10 -freq 100 -m 4 -dumbio -r -v -uart1 stdio

all: check

check: timer_polling timer_interrupt

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(SPARC_CC) $(CFLAGS) $(CONFIG) $(DEFS) $(INCL) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_FILE)

.PHONY: clean

.DEFAULT_GOAL := all

timer_polling: $(TIMER_POLLING)
	$(CCLINK) $(TIMER_POLLING) $(STATIC_LIBS) $(LDFLAGS) -o $(BUILD_DIR)/$(TEST)
	$(SIS_BINARY) $(SIS_PARAMETERS) $(BUILD_DIR)/$(TEST) > $(OUTPUT_FILE)
	grep -q "Success" $(OUTPUT_FILE)
	rm -rf $(BUILD_DIR)/* $(OUTPUT_FILE)

timer_interrupt: $(TIMER_INTERRUPT)
	$(CCLINK) $(TIMER_INTERRUPT) $(STATIC_LIBS) $(LDFLAGS) -o $(BUILD_DIR)/$(TEST)
	$(SIS_BINARY) $(SIS_PARAMETERS) $(BUILD_DIR)/$(TEST) > $(OUTPUT_FILE)
	grep -q "Success" $(OUTPUT_FILE)
	rm -rf $(BUILD_DIR)/* $(OUTPUT_FILE)
