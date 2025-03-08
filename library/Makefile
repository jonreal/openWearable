# OpenWearable Library Makefile
# Builds libowpru.a (PRU drivers library) and libowcore.a (CPU drivers library)

# openWearable path
OW_DIR = /root/openWearable

# Build Directories
LIB_DIR = $(OW_DIR)/library
TARGET_DIR = $(LIB_DIR)/lib
BUILD_DIR = $(LIB_DIR)/build

# Include Directories
CINC_DIR = $(LIB_DIR)/include \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include

PRU_CINC_DIR = -I$(LIB_DIR)/include/ \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include \
	-I/usr/share/ti/cgt-pru/include

# Library dependencies
CLIB = -lpthread -lm
PRU_CLIB = -l/usr/share/ti/cgt-pru/lib/libc.a

# PRU source files (excluding main files)
PRU_SRC = $(filter-out $(LIB_DIR)/pru_src/pru0_main.c $(LIB_DIR)/pru_src/pru1_main.c, $(wildcard $(LIB_DIR)/pru_src/*.c))
PRU_OBJ = $(patsubst $(LIB_DIR)/pru_src/%,$(BUILD_DIR)/%,$(PRU_SRC:.c=.po))

# CPU source files (excluding main.c)
CPU_SRC = $(filter-out $(LIB_DIR)/src/main.c, $(wildcard $(LIB_DIR)/src/*.c))
CPU_OBJ = $(patsubst $(LIB_DIR)/src/%,$(BUILD_DIR)/%,$(CPU_SRC:.c=.o))

# Libraries
LIBOWPRU = $(TARGET_DIR)/libowpru.a
LIBOWCORE = $(TARGET_DIR)/libowcore.a

# Compiler settings
CC = gcc
CFLAGS = -std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a+fp -D=am335x

PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4 -D=am335x -D=FIXMATH_NO_CACHE -D=PRU --hardware_mac=on --gen_func_subsections=on

# Archiver
AR = ar
ARFLAGS = rcs

# Main targets
.PHONY: all clean libowpru libowcore

all: prepare_build libowpru libowcore

prepare_build:
	@echo "\n Creating build directories..."
	@mkdir -p $(BUILD_DIR) $(TARGET_DIR)

# Build libowpru.a - PRU drivers library
libowpru: $(LIBOWPRU)

$(LIBOWPRU): $(PRU_OBJ)
	@echo "\n Building libowpru.a..."
	@mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Build libowcore.a - CPU library
libowcore: $(LIBOWCORE)

$(LIBOWCORE): $(CPU_OBJ)
	@echo "\n Building libowcore.a..."
	@mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Build PRU objects
$(BUILD_DIR)/%.po: $(LIB_DIR)/pru_src/%.c
	@echo "\n Building PRU object: $@..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# Build CPU objects
$(BUILD_DIR)/%.o: $(LIB_DIR)/src/%.c
	@echo "\n Building CPU object: $@..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $<

# Clean
clean:
	@echo "\n Cleaning library build..."
	@rm -rf $(BUILD_DIR) $(TARGET_DIR)

# Helper for debugging
debug:
	@echo " LIB_DIR = $(LIB_DIR)"
	@echo " BUILD_DIR = $(BUILD_DIR)"
	@echo " TARGET_DIR = $(TARGET_DIR)"
	@echo " PRU_SRC = $(PRU_SRC)"
	@echo " PRU_OBJ = $(PRU_OBJ)"
	@echo " CPU_SRC = $(CPU_SRC)"
	@echo " CPU_OBJ = $(CPU_OBJ)"
