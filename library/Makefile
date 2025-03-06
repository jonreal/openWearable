# OpenWearable Library Makefile
# Creates static libraries for CPU and PRU components

# openWearable path
OW_DIR = /root/openWearable

# Build Directories
LIB_DIR = $(OW_DIR)/library
BUILD_DIR = $(LIB_DIR)/build
TARGET_DIR = $(LIB_DIR)/lib

# CPU Library
CPU_LIB = libopenwearable.a
CPU_SRC_DIR = $(LIB_DIR)/src
CPU_INC_DIR = $(LIB_DIR)/include \
		-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
		-I/usr/lib/ti/pru-software-support-package-v6.3/include
CPU_SRC = $(filter-out $(CPU_SRC_DIR)/main.c, $(wildcard $(CPU_SRC_DIR)/*.c))
CPU_OBJ = $(patsubst $(CPU_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CPU_SRC))

# PRU Library
PRU_LIB = libopenwearable_pru.a
PRU_SRC_DIR = $(LIB_DIR)/pru_src
PRU_INC_DIR = -I$(LIB_DIR)/include/ \
		-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
		-I/usr/lib/ti/pru-software-support-package-v6.3/include \
		-I/usr/share/ti/cgt-pru/include
PRU_SRC = $(filter-out $(PRU_SRC_DIR)/pru0_main.c $(PRU_SRC_DIR)/pru1_main.c, $(wildcard $(PRU_SRC_DIR)/*.c))
PRU_OBJ = $(patsubst $(PRU_SRC_DIR)/%.c,$(BUILD_DIR)/%.po,$(PRU_SRC))

# Compiler
CC = gcc
CFLAGS = -std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a+fp -D=am335x
AR = ar
ARFLAGS = rcs

# PRU compiler
PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4 -D=am335x -D=FIXMATH_NO_CACHE -D=PRU --hardware_mac=on --gen_func_subsections=on
PRU_AR = ar
PRU_ARFLAGS = rcs

# Main targets
.PHONY: all clean install

all: cpu_lib pru_lib

cpu_lib: $(TARGET_DIR)/$(CPU_LIB)

pru_lib: $(TARGET_DIR)/$(PRU_LIB)

install: all
	@echo "Library installation complete."

# Build CPU library
$(TARGET_DIR)/$(CPU_LIB): $(CPU_OBJ)
	@echo "Building CPU library..."
	@mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Build PRU library
$(TARGET_DIR)/$(PRU_LIB): $(PRU_OBJ)
	@echo "Building PRU library..."
	@mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Build CPU objects
$(BUILD_DIR)/%.o: $(CPU_SRC_DIR)/%.c
	@echo "Building CPU object: $@"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CPU_INC_DIR) -c -o $@ $<

# Build PRU objects
$(BUILD_DIR)/%.po: $(PRU_SRC_DIR)/%.c
	@echo "Building PRU object: $@"
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_INC_DIR) -c -fe $@ $<

# Clean
clean:
	@echo "Cleaning library build..."
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(TARGET_DIR)

# Helper for debugging
debug:
	@echo "CPU_SRC = $(CPU_SRC)"
	@echo "CPU_OBJ = $(CPU_OBJ)"
	@echo "PRU_SRC = $(PRU_SRC)"
	@echo "PRU_OBJ = $(PRU_OBJ)"