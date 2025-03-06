# Template App Makefile

# App name
APP_NAME = template

# openWearable path
OW_DIR = /root/openWearable

# Build Directories
BUILD_DIR = $(APP_NAME)/build
TARGET_DIR = $(APP_NAME)/bin

# Library Directories
LIB_DIR = $(OW_DIR)/library
LIB_TARGET_DIR = $(LIB_DIR)/lib

# C Source Directories
APP_DIR = $(OW_DIR)/apps/$(APP_NAME)
CORE_SRC_DIR = $(LIB_DIR)/src
PRU_SRC_DIR = $(LIB_DIR)/pru_src

# C Include Directories
CPU_INC_DIR = $(OW_DIR)/library/include \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include \
	-I$(APP_DIR)

# PRU Include Directories
PRU_INC_DIR = -I$(OW_DIR)/library/include/ \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include \
	-I/usr/share/ti/cgt-pru/include \
	-I$(APP_DIR)

# Library dependencies
CLIB = -lpthread -lm
PRU_CLIB = -l/usr/share/ti/cgt-pru/lib/libc.a

# Targets
TARGET = main
PRU0_TARGET = pru0_main
PRU1_TARGET = pru1_main

# App source files
APP_CPU_SRC = $(APP_DIR)/format.c $(APP_DIR)/uiloop.c $(APP_DIR)/cpuloop.c
APP_CPU_OBJ = $(patsubst $(APP_DIR)/%.c,$(BUILD_DIR)/%.o,$(APP_CPU_SRC))

# App PRU source files
APP_PRU_SRC = $(APP_DIR)/pruloop.c

# Core library source files used by this app
CORE_CPU_SRC = $(CORE_SRC_DIR)/pru.c \
               $(CORE_SRC_DIR)/log.c \
               $(CORE_SRC_DIR)/ui.c \
               $(CORE_SRC_DIR)/udp.c \
               $(CORE_SRC_DIR)/debug.c \
               $(CORE_SRC_DIR)/gpio.c \
               $(CORE_SRC_DIR)/fix16.c \
               $(CORE_SRC_DIR)/bno055_arm.c \
               $(CORE_SRC_DIR)/rc_i2c.c

CORE_CPU_OBJ = $(patsubst $(CORE_SRC_DIR)/%.c,$(BUILD_DIR)/core_%.o,$(CORE_CPU_SRC))

# PRU driver source files
PRU_DRIVER_SRC = $(PRU_SRC_DIR)/adcdriver.c \
                 $(PRU_SRC_DIR)/spidriver.c \
                 $(PRU_SRC_DIR)/i2cdriver.c \
                 $(PRU_SRC_DIR)/hsi2c.c \
                 $(PRU_SRC_DIR)/pwmdriver.c \
                 $(PRU_SRC_DIR)/tsc_adc.c \
                 $(PRU_SRC_DIR)/fix16.c \
                 $(PRU_SRC_DIR)/fix16_exp.c \
                 $(PRU_SRC_DIR)/filter.c \
                 $(PRU_SRC_DIR)/bno055.c \
                 $(PRU_SRC_DIR)/encoder.c \
                 $(PRU_SRC_DIR)/halleffect.c \
                 $(PRU_SRC_DIR)/hscsann150pg2a3.c \
                 $(PRU_SRC_DIR)/hx711.c \
                 $(PRU_SRC_DIR)/pca9548a.c \
                 $(PRU_SRC_DIR)/lut.c

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
.PHONY: all clean install prepare_lib

all: prepare_lib build_app

# Main build target for app
build_app: $(APP_CPU_OBJ) $(CORE_CPU_OBJ) $(TARGET_DIR)/$(APP_NAME) $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf

# Ensure library source files are available
prepare_lib:
	@echo "Preparing library source files..."
	@$(MAKE) -C $(LIB_DIR)

# Clean app and library
clean:
	@echo "Cleaning $(APP_NAME)..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(TARGET_DIR)
	@rm -rf /lib/firmware/am335x-pru0-$(APP_NAME)-fw
	@rm -rf /lib/firmware/am335x-pru1-$(APP_NAME)-fw

# Install firmware
install: all
	@echo "Installing PRU firmware..."
	$(INSTALLDIR) /etc/modprobe.d
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU0_TARGET).elf /lib/firmware/am335x-pru0-$(APP_NAME)-fw
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU1_TARGET).elf /lib/firmware/am335x-pru1-$(APP_NAME)-fw

# Build main app executable
$(TARGET_DIR)/$(APP_NAME): $(APP_CPU_OBJ) $(CORE_CPU_OBJ)
	@echo "Building $(APP_NAME) application..."
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -I$(CPU_INC_DIR) $^ $(CORE_SRC_DIR)/main.c -o $@ $(CLIB) -DFWSUFFIX=\"$(APP_NAME)\"

# Build PRU0 firmware
$(BUILD_DIR)/$(PRU0_TARGET).elf: $(APP_DIR)/pruloop.c
	@echo "Building PRU0 firmware..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_INC_DIR) $^ $(PRU_DRIVER_SRC) $(PRU_SRC_DIR)/pru0_main.c -z --heap_size=1024 $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU0_TARGET).map -o $@ $(PRU_SRC_DIR)/PRU0.cmd

# Build PRU1 firmware
$(BUILD_DIR)/$(PRU1_TARGET).elf: $(APP_DIR)/pruloop.c
	@echo "Building PRU1 firmware..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_INC_DIR) $^ $(PRU_DRIVER_SRC) $(PRU_SRC_DIR)/pru1_main.c -z --heap_size=1024 $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU1_TARGET).map -o $@ $(PRU_SRC_DIR)/PRU1.cmd

# Build app CPU objects
$(BUILD_DIR)/%.o: $(APP_DIR)/%.c
	@echo "Building app CPU object: $@"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CPU_INC_DIR) -c -o $@ $<

# Build core library CPU objects
$(BUILD_DIR)/core_%.o: $(CORE_SRC_DIR)/%.c
	@echo "Building core CPU object: $@"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CPU_INC_DIR) -c -o $@ $<

# No intermediate PRU object files - direct compilation