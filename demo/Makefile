BUILD_DIR = ../build
SRC_DIR = ../src
INCL = $(SRC_DIR)/ $(sort $(dir $(wildcard $(SRC_DIR)/**/*.h)))
CFLAGS = $(shell pkg-config --cflags $(PKG_CONFIG)) $(addprefix -I,$(INCL))

all: main.o

main.o : main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f main.o

.PHONY: clean

.DEFAULT_GOAL := all
