# Template App Makefile

# Colors
RED=\033[2;31m
C1=\033[2;32m
C2=\033[2;34m
RESET=\033[0m

# App name
APP_NAME = template

# Platform identity: drives the -D<ARCH> define and the PRUx_<ARCH>.cmd / R5F0_<ARCH>.ld linkers
ARCH = j721e

# PSSP ICSSG header dir. The PRU subsystem (ICSSG) is shared across K3 devices; in this
# PSSP version the complete header set lives under am64x/ (j721e/ is incomplete), so the
# j721e build uses the am64x headers. Other archs use their own dir.
ifeq ($(ARCH),j721e)
PSSP_ARCH = am64x
else
PSSP_ARCH = $(ARCH)
endif

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
LIBOWPRU = $(LIB_DIR)/lib/libow_pru.a

# Source Directories
CSRC_DIR = $(OW_DIR)/library/a72/src
PRU_CSRC_DIR = $(OW_DIR)/library/pru/src
R5F_CSRC_DIR = $(OW_DIR)/library/r5f/src

# Main targets (PRU pair + one lockstep R5F core)
TARGET = main
PRU0_TARGET = pru0_main
PRU1_TARGET = pru1_main
R5F0_TARGET = r5f0_main

# Firmware image names -- convention j7-<proc>-<app>-fw.ow (.ow = openWearable)
PRU0_FW = j7-pru0-$(APP_NAME)-fw.ow
PRU1_FW = j7-pru1-$(APP_NAME)-fw.ow
R5F0_FW = j7-r5f0-$(APP_NAME)-fw.ow

# Include Directories
CINC_DIR = $(OW_DIR)/library/common/include \
	-I$(OW_DIR)/library/a72/include \
	-I$(PSSP_DIR)/include/$(PSSP_ARCH) \
	-I$(PSSP_DIR)/include \
	-I./

PRU_CINC_DIR = -I$(OW_DIR)/library/common/include \
	-I$(OW_DIR)/library/pru/include \
	-I$(PSSP_DIR)/include/$(PSSP_ARCH) \
	-I$(PSSP_DIR)/include \
	-I$(CGT_DIR)/include \
	-I./

R5F_CINC_DIR = -I$(OW_DIR)/library/r5f/include \
	-I$(OW_DIR)/library/common/include \
	-I./

# Library dependencies
CLIB = -lpthread -lm
PRU_CLIB = -l$(CGT_DIR)/lib/libc.a

# CPU source files (excluding main.c)
CPU_SRC = $(filter-out $(CSRC_DIR)/$(TARGET).c, $(wildcard $(CSRC_DIR)/*.c))
CPU_OBJ = $(patsubst $(CSRC_DIR)/%,$(BUILD_DIR)/%,$(CPU_SRC:.c=.o))

# App specific CPU source files
APP_CPU_SRC = format.c uiloop.c cpuloop.c nnloop.c
APP_CPU_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(APP_CPU_SRC))

# App specific PRU source files
APP_PRU_SRC = pruloop.c
APP_PRU_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.po,$(APP_PRU_SRC))

# App specific R5F source files (the hooks, like pruloop.c for the PRU)
APP_R5F_SRC = r5floop.c

# Main object (for main.c)
MAIN_OBJ = $(BUILD_DIR)/$(TARGET).o

# Compiler settings
CC = gcc
ifeq ($(ARCH),am335x)
CFLAGS = \
	-std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a+fp \
	-D$(shell echo $(ARCH) | tr a-z A-Z)
else
CFLAGS = \
	-std=gnu99 -Wall -g -O4 -mtune=cortex-a72 -march=armv8-a \
	-D$(shell echo $(ARCH) | tr a-z A-Z)
endif

PRU_CC = clpru
PRU_CFLAGS = \
	-q --c99 -v3 -O4 \
	-D$(shell echo $(ARCH) | tr a-z A-Z) -DFIXMATH_NO_CACHE -DPRU\
	--hardware_mac=on --gen_func_subsections=on

# R5F: Cortex-R5F bare-metal (arm-none-eabi-gcc); code in ATCM, MPU off, OW crt0
R5F_CC = arm-none-eabi-gcc
R5F_CFLAGS = \
	-mcpu=cortex-r5 -marm -mfloat-abi=soft -ffreestanding -nostdlib \
	-nostartfiles -O2 -Wall $(R5F_CINC_DIR)

# Install stuff
INSTALL = install -m 755
INSTALLDIR = install -d -m 755
INSTALLNONEXEC = install -m 644

# Build targets
.PHONY: all clean install check_libs

all: check_libs $(TARGET_DIR)/$(APP_NAME) $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf $(BUILD_DIR)/$(R5F0_FW) install

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
	@echo "$(C2) $(RM) -r $(TARGET_DIR) $(RESET)"; $(RM) -r $(TARGET_DIR)
	@echo "$(C2) $(RM) firmware $(RESET)"; $(RM) /lib/firmware/$(PRU0_FW) /lib/firmware/$(PRU1_FW) /lib/firmware/$(R5F0_FW)

# Install firmware
install: $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf $(BUILD_DIR)/$(R5F0_FW)
	@echo "$(C1) \n Installing firmware... $(RESET)"
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU0_TARGET).elf /lib/firmware/$(PRU0_FW)
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU1_TARGET).elf /lib/firmware/$(PRU1_FW)
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(R5F0_FW) /lib/firmware/$(R5F0_FW)

# Link arm main app executable
$(TARGET_DIR)/$(APP_NAME): $(CPU_OBJ) $(APP_CPU_OBJ) $(MAIN_OBJ)
	@echo "$(C1) \n Linking cpu... $(RESET)"
	@mkdir -p $(TARGET_DIR)
	$(CC) -Wl,-rpath,$(BUILD_DIR) -L$(BUILD_DIR) $(CFLAGS) $^ -o $@ $(CLIB) -lm -DFWSUFFIX=\"$(APP_NAME)\"

# Build rule for main.c
$(BUILD_DIR)/$(TARGET).o: $(CSRC_DIR)/$(TARGET).c
	@echo "$(C1) \n Building main... $(RESET)"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $< $(CLIB) -DFWSUFFIX=\"$(APP_NAME)\"

# Build PRU0 firmware
$(BUILD_DIR)/$(PRU0_TARGET).elf: $(BUILD_DIR)/$(PRU0_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU)
	@echo "$(C1) \n Linking pru0... $(RESET)"
	$(PRU_CC) $(PRU_CFLAGS) -z --heap_size=1024 $(BUILD_DIR)/$(PRU0_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU) $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU0_TARGET).map -o $@ ./PRU0_$(ARCH).cmd

# Build PRU1 firmware
$(BUILD_DIR)/$(PRU1_TARGET).elf: $(BUILD_DIR)/$(PRU1_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU)
	@echo "$(C1) \n Linking pru1... $(RESET)"
	$(PRU_CC) $(PRU_CFLAGS) -z --heap_size=1024 $(BUILD_DIR)/$(PRU1_TARGET).po $(APP_PRU_OBJ) $(LIBOWPRU) $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU1_TARGET).map -o $@ ./PRU1_$(ARCH).cmd

# Build R5F0 firmware (lockstep core: r5f0_main + app hooks + crt0; single GNU ld step)
$(BUILD_DIR)/$(R5F0_FW): $(R5F_CSRC_DIR)/$(R5F0_TARGET).c $(R5F_CSRC_DIR)/r5f_start.S $(APP_R5F_SRC) R5F0_$(ARCH).ld
	@echo "$(C1) \n Linking r5f0... $(RESET)"
	@mkdir -p $(BUILD_DIR)
	$(R5F_CC) $(R5F_CFLAGS) -T R5F0_$(ARCH).ld $(R5F_CSRC_DIR)/$(R5F0_TARGET).c $(R5F_CSRC_DIR)/r5f_start.S $(APP_R5F_SRC) -o $@

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
	@echo "$(C1) CPU_SRC = $(CPU_SRC) $(RESET)"
	@echo "$(C1) APP_CPU_SRC = $(APP_CPU_SRC) $(RESET)"
	@echo "$(C1) APP_R5F_SRC = $(APP_R5F_SRC) $(RESET)"
	@echo "$(C1) firmware = $(PRU0_FW) $(PRU1_FW) $(R5F0_FW) $(RESET)"
