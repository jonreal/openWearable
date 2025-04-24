# OpenWearable Library Makefile
# Builds libowpru.a (PRU drivers library)

# Colors
RED=\033[1;31m
C1=\033[2;90m
C2=\033[2;34m
RESET=\033[0m

# openWearable path
OW_DIR = /root/openWearable

# Build Directories
LIB_DIR = $(OW_DIR)/library
TARGET_DIR = $(LIB_DIR)/lib
BUILD_DIR = $(LIB_DIR)/build

# Include Directories
PRU_CINC_DIR = -I$(LIB_DIR)/include

# Library dependencies
PRU_CLIB = -l$(CGT_DIR)/lib/libc.a

# PRU source files (excluding main files)
PRU_SRC = $(filter-out $(LIB_DIR)/pru_src/pru0_main.c $(LIB_DIR)/pru_src/pru1_main.c, $(wildcard $(LIB_DIR)/pru_src/*.c))
PRU_OBJ = $(patsubst $(LIB_DIR)/pru_src/%,$(BUILD_DIR)/%,$(PRU_SRC:.c=.po))

# Libraries
LIBOWPRU = $(TARGET_DIR)/libowpru.a

# PRU Compiler settings
PRU_CC = clpru
PRU_CFLAGS = \
	-q --c99 -v3 -O4  \
	-DFIXMATH_NO_CACHE \
	--hardware_mac=on --gen_func_subsections=on

# Archiver
AR = ar
ARFLAGS = rcs

# Main targets
.PHONY: all clean libowpru

all: prepare_build libowpru

prepare_build:
	@echo "$(C2) \n Creating build directories... $(RESET)"
	@mkdir -p $(BUILD_DIR) $(TARGET_DIR)

# Build libowpru.a - PRU drivers library
libowpru: $(LIBOWPRU)

$(LIBOWPRU): $(PRU_OBJ)
	@echo "$(C2) \n Building libowpru.a... $(RESET)"
	@mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Build PRU objects
$(BUILD_DIR)/%.po: $(LIB_DIR)/pru_src/%.c
	@echo "$(C2) \n Building PRU object: $@... $(RESET)"
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# Clean
clean:
	@echo "$(C1) \n Cleaning library build... $(RESET)"
	@echo "$(C1) $(RM) -r $(BUILD_DIR) $(TARGET_DIR) $(RESET)"; $(RM) -r $(BUILD_DIR) $(TARGET_DIR)

# Helper for debugging
debug:
	@echo " LIB_DIR = $(LIB_DIR)"
	@echo " BUILD_DIR = $(BUILD_DIR)"
	@echo " TARGET_DIR = $(TARGET_DIR)"
	@echo " PRU_SRC = $(PRU_SRC)"
	@echo " PRU_OBJ = $(PRU_OBJ)"
