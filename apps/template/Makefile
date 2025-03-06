# Template App Makefile

# App name
APP_NAME = template

# openWearable path
OW_DIR = /root/openWearable

# Build Directories
BUILD_DIR = $(APP_NAME)/build
TARGET_DIR = $(APP_NAME)/bin

# C Source Directories
APP_DIR = $(OW_DIR)/apps/$(APP_NAME)
CPU_LIB_DIR = $(OW_DIR)/library/lib
CPU_INC_DIR = $(OW_DIR)/library/include \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include
CLIB = -lpthread

# PRU Source Directories
PRU_LIB_DIR = $(OW_DIR)/library/lib
PRU_INC_DIR = -I$(OW_DIR)/library/include/ \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include \
	-I/usr/share/ti/cgt-pru/include
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

# Compiler
CC = gcc
CFLAGS = -std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a+fp -D=am335x
LDFLAGS = -L$(CPU_LIB_DIR) -lopenwearable -lm

# PRU compiler
PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4 -D=am335x -D=FIXMATH_NO_CACHE -D=PRU --hardware_mac=on --gen_func_subsections=on
PRU_LDFLAGS = -L$(PRU_LIB_DIR) -lopenwearable_pru

# Install stuff
INSTALL = install -m 755
INSTALLDIR = install -d -m 755
INSTALLNONEXEC = install -m 644

# Build targets
.PHONY: all clean install

all: $(TARGET_DIR)/$(APP_NAME) $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf

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
$(TARGET_DIR)/$(APP_NAME): $(APP_CPU_OBJ) | build_library
	@echo "Building $(APP_NAME) application..."
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -I$(CPU_INC_DIR) -I$(APP_DIR) $^ $(OW_DIR)/library/src/main.c -o $@ $(LDFLAGS) $(CLIB) -DFWSUFFIX=\"$(APP_NAME)\"

# Build PRU0 firmware
$(BUILD_DIR)/$(PRU0_TARGET).elf: $(APP_PRU_OBJ) $(OW_DIR)/library/pru_src/pru0_main.c | build_library
	@echo "Building PRU0 firmware..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_INC_DIR) -I$(APP_DIR) $(APP_PRU_OBJ) $(OW_DIR)/library/pru_src/pru0_main.c -z --heap_size=1024 $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU0_TARGET).map -o $@ $(OW_DIR)/library/pru_src/PRU0.cmd $(PRU_LDFLAGS)

# Build PRU1 firmware
$(BUILD_DIR)/$(PRU1_TARGET).elf: $(APP_PRU_OBJ) $(OW_DIR)/library/pru_src/pru1_main.c | build_library
	@echo "Building PRU1 firmware..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_INC_DIR) -I$(APP_DIR) $(APP_PRU_OBJ) $(OW_DIR)/library/pru_src/pru1_main.c -z --heap_size=1024 $(PRU_CLIB) -m $(BUILD_DIR)/$(PRU1_TARGET).map -o $@ $(OW_DIR)/library/pru_src/PRU1.cmd $(PRU_LDFLAGS)

# Build app CPU objects
$(BUILD_DIR)/%.o: $(APP_DIR)/%.c
	@echo "Building CPU object: $@"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CPU_INC_DIR) -I$(APP_DIR) -c -o $@ $<

# Build app PRU objects
$(BUILD_DIR)/%.po: $(APP_DIR)/%.c
	@echo "Building PRU object: $@"
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_INC_DIR) -I$(APP_DIR) -c -fe $@ $<

# Ensure library is built before app
build_library:
	@echo "Building libraries..."
	$(MAKE) -C $(OW_DIR)/library all