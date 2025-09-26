# Template App Makefile

# Colors
RED=\033[2;31m
C1=\033[2;32m
C2=\033[2;34m
RESET=\033[0m

# App name
APP_NAME = close-all-valves

# Architecture
ARCH = am335x

# openWearable path
OW_DIR = /root/openWearable

# TI pru software support package
PSSP_DIR = /root/pru-software-support-package
CGT_DIR = /usr/share/ti/cgt-pru

# Build Directories
BUILD_DIR = ./build
TARGET_DIR = /root/openWearable/apps/bin

# Library Directories
LIB_DIR = $(OW_DIR)/library
LIBOWPRU = $(LIB_DIR)/lib/libowpru.a

# Source Directories
CSRC_DIR = $(OW_DIR)/library/src
PRU_CSRC_DIR = $(OW_DIR)/library/pru_src

# Main targets
TARGET = main
PRU0_TARGET = pru0_main
PRU1_TARGET = pru1_main

# Include Directories
CINC_DIR = $(OW_DIR)/library/include \
	-I$(PSSP_DIR)/include/$(ARCH) \
	-I$(PSSP_DIR)/include \
	-I./

PRU_CINC_DIR = -I$(OW_DIR)/library/include/ \
	-I$(PSSP_DIR)/include/$(ARCH) \
	-I$(PSSP_DIR)/include \
	-I$(CGT_DIR)/include \
	-I./

# Library dependencies
CLIB = -lpthread -lm
PRU_CLIB = -l$(CGT_DIR)/lib/libc.a

# CPU source files (excluding main.c)
CPU_SRC = $(filter-out $(CSRC_DIR)/$(TARGET).c, $(wildcard $(CSRC_DIR)/*.c))
CPU_OBJ = $(patsubst $(CSRC_DIR)/%,$(BUILD_DIR)/%,$(CPU_SRC:.c=.o))

# App specific CPU source files
APP_CPU_SRC = format.c uiloop.c cpuloop.c
APP_CPU_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(APP_CPU_SRC))

# App specific PRU source files
APP_PRU_SRC = pruloop.c
APP_PRU_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.po,$(APP_PRU_SRC))

# Compiler settings
CC = gcc
CFLAGS = \
	-std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a+fp \
	-D$(shell echo $(ARCH) | tr a-z A-Z)

PRU_CC = clpru
PRU_CFLAGS = \
	-q --c99 -v3 -O4 \
	-D$(shell echo $(ARCH) | tr a-z A-Z) -DFIXMATH_NO_CACHE -DPRU\
	--hardware_mac=on --gen_func_subsections=on

# Install stuff
INSTALL = install -m 755
INSTALLDIR = install -d -m 755
INSTALLNONEXEC = install -m 644

# Build targets
.PHONY: all clean install check_libs

all: check_libs $(TARGET_DIR)/$(APP_NAME) $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf install

# Check if required libraries exist
check_libs:
	@if [ ! -f $(LIBOWPRU) ]; then \
		echo "$(RED) \n ERROR: $(LIBOWPRU) not found. $(RESET)"; \
		echo "$(C1) \n Please build PRU library first with 'make' in $(LIB_DIR) directory. $(RESET)"; \
		exit 1; \
	fi

# Clean app
clean:
	@echo "$(C1) \n Cleaning $(APP_NAME)... $(RESET)"
	@echo "$(C2) $(RM) -r ./build $(RESET)"; $(RM) -r ./build
	@echo "$(C2) $(RM) -r $(TARGET_DIR)/$(APP_NAME) $(RESET)"; $(RM) -r $(TARGET_DIR)/$(APP_NAME)
	@echo "$(C2) $(RM) -r /lib/firmware/$(ARCH)-pru0-$(APP_NAME)-fw $(RESET)"; $(RM) -r /lib/firmware/$(ARCH)-pru0-$(APP_NAME)-fw
	@echo "$(C2) $(RM) -r /lib/firmware/$(ARCH)-pru1-$(APP_NAME)-fw $(RESET)"; $(RM) -r /lib/firmware/$(ARCH)-pru1-$(APP_NAME)-fw

# Install firmware
install: $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf
	@echo "$(C1) \n Installing PRU firmware... $(RESET)"
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU0_TARGET).elf /lib/firmware/$(ARCH)-pru0-$(APP_NAME)-fw
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU1_TARGET).elf /lib/firmware/$(ARCH)-pru1-$(APP_NAME)-fw

# Build main app executable
$(TARGET_DIR)/$(APP_NAME): $(CPU_OBJ) $(APP_CPU_OBJ)
	@echo "$(C1) \n Linking cpu... $(RESET)"
	@mkdir -p $(TARGET_DIR)
	$(CC) -Wl,-rpath,$(BUILD_DIR) -L$(BUILD_DIR) $(CFLAGS) $^ $(CSRC_DIR)/$(TARGET).c -I$(CINC_DIR) -o $@ $(CLIB) -lm -DFWSUFFIX=\"$(APP_NAME)\"

# Build PRU0 firmware
$(BUILD_DIR)/$(PRU0_TARGET).elf: $(BUILD_DIR)/$(PRU0_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU)
	@echo "$(C1) \n Linking pru0... $(RESET)"
	$(PRU_CC) $(PRU_CFLAGS) -z --heap_size=1024 $(BUILD_DIR)/$(PRU0_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU) $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU0_TARGET).map -o $@ ./PRU0.cmd

# Build PRU1 firmware
$(BUILD_DIR)/$(PRU1_TARGET).elf: $(BUILD_DIR)/$(PRU1_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU)
	@echo "$(C1) \n Linking pru1... $(RESET)"
	$(PRU_CC) $(PRU_CFLAGS) -z --heap_size=1024 $(BUILD_DIR)/$(PRU1_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU) $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU1_TARGET).map -o $@ ./PRU1.cmd

# Build CPU objects
$(BUILD_DIR)/%.o: $(CSRC_DIR)/%.c
	@echo "$(C1) \n Building core dependencies... $(RESET)"
	@mkdir -p $(BUILD_DIR)
	$(CC) -Wl,-rpath,$(BUILD_DIR) -L$(BUILD_DIR) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $< $(CLIB)

# Build app CPU objects
$(BUILD_DIR)/%.o: %.c
	@echo "$(C1) \n Building app specific dependencies... $(RESET)"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $< $(CLIB)

# Build PRU main objects
$(BUILD_DIR)/$(PRU0_TARGET).po: $(PRU_CSRC_DIR)/$(PRU0_TARGET).c
	@echo "$(C1) \n Building pru dependencies... $(RESET)"
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

$(BUILD_DIR)/$(PRU1_TARGET).po: $(PRU_CSRC_DIR)/$(PRU1_TARGET).c
	@echo "$(C1) \n Building pru dependencies... $(RESET)"
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# Build app PRU objects
$(BUILD_DIR)/%.po: %.c
	@echo "$(C1) \n Building app specific pru dependencies... $(RESET)"
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# Helper for debugging
debug:
	@echo "$(C1) APP_NAME = $(APP_NAME) $(RESET)"
	@echo "$(C1) BUILD_DIR = $(BUILD_DIR) $(RESET)"
	@echo "$(C1) TARGET_DIR = $(TARGET_DIR) $(RESET)"
	@echo "$(C1) LIBOWPRU = $(LIBOWPRU) $(RESET)"
	@echo "$(C1) CPU_SRC = $(CPU_SRC) $(RESET)"
	@echo "$(C1) CPU_OBJ = $(CPU_OBJ) $(RESET)"
	@echo "$(C1) APP_CPU_SRC = $(APP_CPU_SRC) $(RESET)"
	@echo "$(C1) APP_CPU_OBJ = $(APP_CPU_OBJ) $(RESET)"
	@echo "$(C1) APP_PRU_SRC = $(APP_PRU_SRC) $(RESET)"
	@echo "$(C1) APP_PRU_OBJ = $(APP_PRU_OBJ) $(RESET)"
