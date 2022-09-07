ROOT_PATH = ../../..

include $(ROOT_PATH)/definitions.mk

TEST := test.exe

CFLAGS = -g -O2 $(shell pkg-config --cflags $(PKG_CONFIG))
CCLINK = $(CC) $(CFLAGS) -Wl,-Map,$(BUILD_DIR)/$(basename $@).map

INCL = $(addprefix -I,$(sort $(dir $(wildcard ./$(ROOT_PATH)/$(SRC_DIR)/$(UART_SRC_DIR)/*.h) \
       $(wildcard ./$(ROOT_PATH)/$(SRC_DIR)/$(UTILS_SRC_DIR)/*.h)  \
       $(wildcard ./$(ROOT_PATH)/$(SRC_DIR)/$(SYSTEM_CONFIG_SRC_DIR)/*.h))))

STATIC_LIBS = -Wl,-Bstatic $(ROOT_PATH)/$(BUILD_DIR)/$(SRC_DIR)/$(UART_SRC_DIR)/libuart.a

UART_WRITE_ASYNC_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o, uart_write_async.c)
UART_READ_ASYNC_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o, uart_read_async.c)
UART_READ_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o, uart_read.c)
UART_WRITE_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o, uart_write.c)

UART_FILE = uart
SIS_BINARY = $(ROOT_PATH)/$(SIS_MODULE_SRC_DIR)/$(BUILD_DIR)/$(SRC_DIR)/$(SIS_NAME)-$(SIS_VERSION)
SIS_PARAMETERS = -leon3 -d 10 -freq 100 -m 4 -r -v -uart1 $(UART_FILE)

all: check

check: uart_read uart_read_async uart_write uart_write_async
# uart_read uart_read_async uart_write uart_write_async

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CONFIG) $(DEFS) $(INCL) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(UART_FILE)

.PHONY: clean

.DEFAULT_GOAL := all

uart_read: $(UART_READ_OBJ)
	$(CCLINK) $(UART_READ_OBJ) $(STATIC_LIBS) $(LDFLAGS) -o $(BUILD_DIR)/$(TEST)
	$(SIS_BINARY) $(SIS_PARAMETERS) $(BUILD_DIR)/$(TEST)
	grep -q "Success" $(UART_FILE)
	rm -rf $(BUILD_DIR)/* $(UART_FILE)

uart_read_async: $(UART_READ_ASYNC_OBJ)
	$(CCLINK) $(UART_READ_ASYNC_OBJ) $(STATIC_LIBS) $(LDFLAGS) -o $(BUILD_DIR)/$(TEST)
	$(SIS_BINARY) $(SIS_PARAMETERS) $(BUILD_DIR)/$(TEST)
	grep -q "Success" $(UART_FILE)
	rm -rf $(BUILD_DIR)/* $(UART_FILE)

uart_write: $(UART_WRITE_OBJ)
	$(CCLINK) $(UART_WRITE_OBJ) $(STATIC_LIBS) $(LDFLAGS) -o $(BUILD_DIR)/$(TEST)
	$(SIS_BINARY) $(SIS_PARAMETERS) $(BUILD_DIR)/$(TEST)
	grep -q "Success" $(UART_FILE)
	rm -rf $(BUILD_DIR)/* $(UART_FILE)

uart_write_async: $(UART_WRITE_ASYNC_OBJ)
	$(CCLINK) $(UART_WRITE_ASYNC_OBJ) $(STATIC_LIBS) $(LDFLAGS) -o $(BUILD_DIR)/$(TEST)
	$(SIS_BINARY) $(SIS_PARAMETERS) $(BUILD_DIR)/$(TEST)
	grep -q "Success" $(UART_FILE)
	rm -rf $(BUILD_DIR)/* $(UART_FILE)
