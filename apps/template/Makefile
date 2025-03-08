# Template App Makefile

# App name
APP_NAME = template

# openWearable path
OW_DIR = /root/openWearable

# Build Directories
BUILD_DIR = ./build
TARGET_DIR = ./bin

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
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include \
	-I./

PRU_CINC_DIR = -I$(OW_DIR)/library/include/ \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include \
	-I/usr/share/ti/cgt-pru/include \
	-I./

# Library dependencies
CLIB = -lpthread -lm
PRU_CLIB = -l/usr/share/ti/cgt-pru/lib/libc.a

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
CFLAGS = -std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a+fp -D=am335x

PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4 -D=am335x -D=FIXMATH_NO_CACHE -D=PRU --hardware_mac=on --gen_func_subsections=on

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
		echo " ERROR: $(LIBOWPRU) not found."; \
		echo " Please build PRU library first with 'make' in $(LIB_DIR) directory."; \
		exit 1; \
	fi

# Clean app
clean:
	@echo " Cleaning $(APP_NAME)..."
	@echo " $(RM) -r ./build"; $(RM) -r ./build
	@echo " $(RM) -r $(TARGET_DIR)"; $(RM) -r $(TARGET_DIR)
	@echo " $(RM) -r /lib/firmware/am335x-pru0-$(APP_NAME)-fw"; $(RM) -r /lib/firmware/am335x-pru0-$(APP_NAME)-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-$(APP_NAME)-fw"; $(RM) -r /lib/firmware/am335x-pru1-$(APP_NAME)-fw

# Install firmware
install: $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf
	@echo " Installing PRU firmware..."
	$(INSTALLDIR) /etc/modprobe.d
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU0_TARGET).elf /lib/firmware/am335x-pru0-$(APP_NAME)-fw
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU1_TARGET).elf /lib/firmware/am335x-pru1-$(APP_NAME)-fw

# Build main app executable
$(TARGET_DIR)/$(APP_NAME): $(CPU_OBJ) $(APP_CPU_OBJ)
	@echo " Linking cpu ..."
	@mkdir -p $(TARGET_DIR)
	$(CC) -Wl,-rpath,$(BUILD_DIR) -L$(BUILD_DIR) $(CFLAGS) $^ $(CSRC_DIR)/$(TARGET).c -I$(CINC_DIR) -o $@ $(CLIB) -lm -DFWSUFFIX=\"$(APP_NAME)\"

# Build PRU0 firmware
$(BUILD_DIR)/$(PRU0_TARGET).elf: $(BUILD_DIR)/$(PRU0_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU)
	@echo " Linking pru0..."
	$(PRU_CC) $(PRU_CFLAGS) -z --heap_size=1024 $(BUILD_DIR)/$(PRU0_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU) $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU0_TARGET).map -o $@ $(PRU_CSRC_DIR)/PRU0.cmd

# Build PRU1 firmware
$(BUILD_DIR)/$(PRU1_TARGET).elf: $(BUILD_DIR)/$(PRU1_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU)
	@echo " Linking pru1..."
	$(PRU_CC) $(PRU_CFLAGS) -z --heap_size=1024 $(BUILD_DIR)/$(PRU1_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU) $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU1_TARGET).map -o $@ $(PRU_CSRC_DIR)/PRU1.cmd

# Build CPU objects
$(BUILD_DIR)/%.o: $(CSRC_DIR)/%.c
	@echo " Building core dependencies..."
	@mkdir -p $(BUILD_DIR)
	$(CC) -Wl,-rpath,$(BUILD_DIR) -L$(BUILD_DIR) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $< $(CLIB)

# Build app CPU objects
$(BUILD_DIR)/%.o: %.c
	@echo " Building app specific dependencies..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $< $(CLIB)

# Build PRU main objects
$(BUILD_DIR)/$(PRU0_TARGET).po: $(PRU_CSRC_DIR)/$(PRU0_TARGET).c
	@echo " Building pru dependencies..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

$(BUILD_DIR)/$(PRU1_TARGET).po: $(PRU_CSRC_DIR)/$(PRU1_TARGET).c
	@echo " Building pru dependencies..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# Build app PRU objects
$(BUILD_DIR)/%.po: %.c
	@echo " Building app specific pru dependencies..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# Helper for debugging
debug:
	@echo " APP_NAME = $(APP_NAME)"
	@echo " BUILD_DIR = $(BUILD_DIR)"
	@echo " TARGET_DIR = $(TARGET_DIR)"
	@echo " LIBOWPRU = $(LIBOWPRU)"
	@echo " CPU_SRC = $(CPU_SRC)"
	@echo " CPU_OBJ = $(CPU_OBJ)"
	@echo " APP_CPU_SRC = $(APP_CPU_SRC)"
	@echo " APP_CPU_OBJ = $(APP_CPU_OBJ)"
	@echo " APP_PRU_SRC = $(APP_PRU_SRC)"
	@echo " APP_PRU_OBJ = $(APP_PRU_OBJ)"