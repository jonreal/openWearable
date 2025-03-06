# OpenWearable Library Makefile
# Prepares source files for app-level compilation

# openWearable path
OW_DIR = /root/openWearable

# Build Directories
LIB_DIR = $(OW_DIR)/library
TARGET_DIR = $(LIB_DIR)/lib

# Main targets
.PHONY: all clean

all: prepare_sources

prepare_sources:
	@echo "Preparing source files for app-level compilation..."
	@mkdir -p $(TARGET_DIR)/include $(TARGET_DIR)/src $(TARGET_DIR)/pru_src
	@cp $(LIB_DIR)/include/*.h $(TARGET_DIR)/include/
	@cp $(LIB_DIR)/src/*.c $(TARGET_DIR)/src/
	@cp $(LIB_DIR)/pru_src/*.c $(TARGET_DIR)/pru_src/
	@cp $(LIB_DIR)/pru_src/*.cmd $(TARGET_DIR)/pru_src/
	@echo "Source files prepared for app-level compilation"

# Clean
clean:
	@echo "Cleaning library build..."
	@rm -rf $(TARGET_DIR)

# Helper for debugging
debug:
	@echo "LIB_DIR = $(LIB_DIR)"
	@echo "TARGET_DIR = $(TARGET_DIR)"