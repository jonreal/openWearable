# OpenWearable Library Makefile
# Creates static libraries for independent components that don't depend on state.h

# openWearable path
OW_DIR = /root/openWearable

# Build Directories
LIB_DIR = $(OW_DIR)/library
BUILD_DIR = $(LIB_DIR)/build
TARGET_DIR = $(LIB_DIR)/lib

# CPU Library - Only include files that don't depend on state.h
CPU_LIB = libowcore.a
CPU_SRC_DIR = $(LIB_DIR)/src
CPU_INC_DIR = $(LIB_DIR)/include \
		-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
		-I/usr/lib/ti/pru-software-support-package-v6.3/include
CPU_INDEPENDENT_SRC = $(CPU_SRC_DIR)/bno055_arm.c \
                      $(CPU_SRC_DIR)/debug.c \
                      $(CPU_SRC_DIR)/fix16.c \
                      $(CPU_SRC_DIR)/gpio.c \
                      $(CPU_SRC_DIR)/rc_i2c.c
CPU_OBJ = $(patsubst $(CPU_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CPU_INDEPENDENT_SRC))

# CPU State-dependent Library - Empty shell that will be filled at app level
CPU_STATE_LIB = libowstate.a
CPU_STATE_SRC = $(CPU_SRC_DIR)/pru.c \
                $(CPU_SRC_DIR)/log.c \
                $(CPU_SRC_DIR)/ui.c
CPU_STATE_OBJ = # Empty - will be compiled at app level with state.h

# PRU Library - Only include files that don't depend on state.h
PRU_LIB = libowpru.a
PRU_SRC_DIR = $(LIB_DIR)/pru_src
PRU_INC_DIR = -I$(LIB_DIR)/include/ \
		-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
		-I/usr/lib/ti/pru-software-support-package-v6.3/include \
		-I/usr/share/ti/cgt-pru/include
		
# PRU drivers and utilities that don't depend on state.h
PRU_INDEPENDENT_SRC = $(PRU_SRC_DIR)/adcdriver.c \
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

PRU_OBJ = $(patsubst $(PRU_SRC_DIR)/%.c,$(BUILD_DIR)/%.po,$(PRU_INDEPENDENT_SRC))

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

all: cpu_lib pru_lib state_dependent_headers

cpu_lib: $(TARGET_DIR)/$(CPU_LIB)

pru_lib: $(TARGET_DIR)/$(PRU_LIB)

# Create directory with essential headers and source for app compilation
state_dependent_headers:
	@echo "Creating directory for state-dependent sources..."
	@mkdir -p $(TARGET_DIR)/include $(TARGET_DIR)/src
	@cp $(LIB_DIR)/include/mem_types.h $(TARGET_DIR)/include/
	@cp $(LIB_DIR)/include/pru.h $(TARGET_DIR)/include/
	@cp $(CPU_STATE_SRC) $(TARGET_DIR)/src/

install: all
	@echo "Library installation complete."

# Build CPU library
$(TARGET_DIR)/$(CPU_LIB): $(CPU_OBJ)
	@echo "Building CPU core library..."
	@mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Build PRU library
$(TARGET_DIR)/$(PRU_LIB): $(PRU_OBJ)
	@echo "Building PRU core library..."
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
	@echo "CPU_INDEPENDENT_SRC = $(CPU_INDEPENDENT_SRC)"
	@echo "CPU_OBJ = $(CPU_OBJ)"
	@echo "CPU_STATE_SRC = $(CPU_STATE_SRC)"
	@echo "PRU_INDEPENDENT_SRC = $(PRU_INDEPENDENT_SRC)"
	@echo "PRU_OBJ = $(PRU_OBJ)"