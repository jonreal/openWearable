# OpenWearable Library Makefile
# Builds the per-core static libraries. Currently: libow_pru.a (PRU drivers).
# Per-core source/include layout: common/  a72/  pru/  (future: r5f/ c7x/ c66x/)

# Colors
RED=\033[1;31m
C1=\033[2;90m
C2=\033[2;34m
RESET=\033[0m

# openWearable path
OW_DIR = /root/openWearable

# TI PRU code-generation tools (for the clpru runtime headers: stdint.h, etc.)
CGT_DIR = /usr/share/ti/cgt-pru

# Build Directories
LIB_DIR = $(OW_DIR)/library
TARGET_DIR = $(LIB_DIR)/lib
BUILD_DIR = $(LIB_DIR)/build

# Include Directories (PRU: shared contract + pru-specific headers + clpru runtime)
PRU_CINC_DIR = -I$(LIB_DIR)/common/include -I$(LIB_DIR)/pru/include -I$(CGT_DIR)/include

# PRU source files (excluding the per-app main files)
PRU_SRC = $(filter-out $(LIB_DIR)/pru/src/pru0_main.c $(LIB_DIR)/pru/src/pru1_main.c, $(wildcard $(LIB_DIR)/pru/src/*.c))
PRU_OBJ = $(patsubst $(LIB_DIR)/pru/src/%,$(BUILD_DIR)/%,$(PRU_SRC:.c=.po))

# Libraries
LIBOWPRU = $(TARGET_DIR)/libow_pru.a

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
.PHONY: all clean libow_pru

all: prepare_build libow_pru

prepare_build:
	@echo "$(C2) \n Creating build directories... $(RESET)"
	@mkdir -p $(BUILD_DIR) $(TARGET_DIR)

# Build libow_pru.a - PRU drivers library
libow_pru: $(LIBOWPRU)

$(LIBOWPRU): $(PRU_OBJ)
	@echo "$(C2) \n Building libow_pru.a... $(RESET)"
	@mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Build PRU objects
$(BUILD_DIR)/%.po: $(LIB_DIR)/pru/src/%.c
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
