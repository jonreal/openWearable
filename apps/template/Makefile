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
STATE_SRC_DIR = $(LIB_TARGET_DIR)/src

# C Source Directories
APP_DIR = $(OW_DIR)/apps/$(APP_NAME)
CPU_INC_DIR = $(OW_DIR)/library/include \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include \
	-I$(APP_DIR)
CLIB = -lpthread

# PRU Source Directories
PRU_INC_DIR = -I$(OW_DIR)/library/include/ \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include \
	-I/usr/share/ti/cgt-pru/include \
	-I$(APP_DIR)
PRU_CLIB = -l/usr/share/ti/cgt-pru/lib/libc.a

# Targets
TARGET = main
PRU0_TARGET = pru0_main
PRU1_TARGET = pru1_main

# App source files
APP_CPU_SRC = $(APP_DIR)/format.c $(APP_DIR)/uiloop.c $(APP_DIR)/cpuloop.c
APP_CPU_OBJ = $(patsubst $(APP_DIR)/%.c,$(BUILD_DIR)/%.o,$(APP_CPU_SRC))
APP_PRU_SRC = $(APP_DIR)/pruloop.c
APP_PRU_OBJ = $(patsubst $(APP_DIR)/%.c,$(BUILD_DIR)/%.po,$(APP_PRU_SRC))

# State-dependent sources (need app's state.h)
STATE_DEPENDENT_SRC = $(STATE_SRC_DIR)/pru.c $(STATE_SRC_DIR)/log.c $(STATE_SRC_DIR)/ui.c $(STATE_SRC_DIR)/udp.c
STATE_DEPENDENT_OBJ = $(patsubst $(STATE_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(STATE_DEPENDENT_SRC))

# Rebuild state-dependent sources before compiling
rebuild_state_sources:
	@echo "Ensuring all state-dependent sources are available..."
	@$(MAKE) -C $(LIB_DIR) all

# Compiler
CC = gcc
CFLAGS = -std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a+fp -D=am335x
LDFLAGS = -L$(LIB_TARGET_DIR) -lowcore -lm

# PRU compiler
PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4 -D=am335x -D=FIXMATH_NO_CACHE -D=PRU --hardware_mac=on --gen_func_subsections=on
PRU_LDFLAGS = -L$(LIB_TARGET_DIR) -lowpru

# Install stuff
INSTALL = install -m 755
INSTALLDIR = install -d -m 755
INSTALLNONEXEC = install -m 644

# Build targets
.PHONY: all clean install rebuild rebuild_state_sources

all: rebuild_state_sources $(TARGET_DIR)/$(APP_NAME) $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf

# Full rebuild including library
rebuild: rebuild_state_sources all

install: all
	@echo "Installing PRU firmware..."
	$(INSTALLDIR) /etc/modprobe.d
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU0_TARGET).elf /lib/firmware/am335x-pru0-$(APP_NAME)-fw
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU1_TARGET).elf /lib/firmware/am335x-pru1-$(APP_NAME)-fw

clean:
	@echo "Cleaning $(APP_NAME)..."
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(TARGET_DIR)
	$(RM) -r /lib/firmware/am335x-pru0-$(APP_NAME)-fw
	$(RM) -r /lib/firmware/am335x-pru1-$(APP_NAME)-fw

# Build main application
$(TARGET_DIR)/$(APP_NAME): $(APP_CPU_OBJ) $(STATE_DEPENDENT_OBJ)
	@echo "Building $(APP_NAME) application..."
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -I$(CPU_INC_DIR) $^ $(LIB_DIR)/src/main.c -o $@ $(LDFLAGS) $(CLIB) -DFWSUFFIX=\"$(APP_NAME)\"

# Build PRU0 firmware
$(BUILD_DIR)/$(PRU0_TARGET).elf: $(APP_PRU_OBJ)
	@echo "Building PRU0 firmware..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_INC_DIR) $(APP_PRU_OBJ) $(LIB_DIR)/pru_src/pru0_main.c -z --heap_size=1024 $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU0_TARGET).map -o $@ $(LIB_DIR)/pru_src/PRU0.cmd $(PRU_LDFLAGS)

# Build PRU1 firmware
$(BUILD_DIR)/$(PRU1_TARGET).elf: $(APP_PRU_OBJ)
	@echo "Building PRU1 firmware..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_INC_DIR) $(APP_PRU_OBJ) $(LIB_DIR)/pru_src/pru1_main.c -z --heap_size=1024 $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU1_TARGET).map -o $@ $(LIB_DIR)/pru_src/PRU1.cmd $(PRU_LDFLAGS)

# Build app CPU objects
$(BUILD_DIR)/%.o: $(APP_DIR)/%.c
	@echo "Building app CPU object: $@"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CPU_INC_DIR) -c -o $@ $<

# Build state-dependent objects with access to app's state.h
$(BUILD_DIR)/%.o: $(STATE_SRC_DIR)/%.c
	@echo "Building state-dependent object: $@"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CPU_INC_DIR) -c -o $@ $<

# Build app PRU objects
$(BUILD_DIR)/%.po: $(APP_DIR)/%.c
	@echo "Building app PRU object: $@"
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_INC_DIR) -c -fe $@ $<

# Removed build_library target (replaced by rebuild_state_sources)