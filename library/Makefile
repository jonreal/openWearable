# OpenWearable Library Makefile
# Builds libowpru.a (PRU drivers library)

# openWearable path
OW_DIR = /root/openWearable

# Build Directories
LIB_DIR = $(OW_DIR)/library
TARGET_DIR = $(LIB_DIR)/lib
BUILD_DIR = $(LIB_DIR)/build

# Include Directories
PRU_CINC_DIR = -I$(LIB_DIR)/include/ \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include/am335x \
	-I/usr/lib/ti/pru-software-support-package-v6.3/include \
	-I/usr/share/ti/cgt-pru/include

# Library dependencies
PRU_CLIB = -l/usr/share/ti/cgt-pru/lib/libc.a

# PRU source files (excluding main files)
PRU_SRC = $(filter-out $(LIB_DIR)/pru_src/pru0_main.c $(LIB_DIR)/pru_src/pru1_main.c, $(wildcard $(LIB_DIR)/pru_src/*.c))
PRU_OBJ = $(patsubst $(LIB_DIR)/pru_src/%,$(BUILD_DIR)/%,$(PRU_SRC:.c=.po))

# Libraries
LIBOWPRU = $(TARGET_DIR)/libowpru.a

# PRU Compiler settings
PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4 -D=am335x -D=FIXMATH_NO_CACHE -D=PRU --hardware_mac=on --gen_func_subsections=on

# Archiver
AR = ar
ARFLAGS = rcs

# Main targets
.PHONY: all clean libowpru

all: prepare_build libowpru

prepare_build:
	@echo "\n Creating build directories..."
	@mkdir -p $(BUILD_DIR) $(TARGET_DIR)

# Build libowpru.a - PRU drivers library
libowpru: $(LIBOWPRU)

$(LIBOWPRU): $(PRU_OBJ)
	@echo "\n Building libowpru.a..."
	@mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Build PRU objects
$(BUILD_DIR)/%.po: $(LIB_DIR)/pru_src/%.c
	@echo "\n Building PRU object: $@..."
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

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
