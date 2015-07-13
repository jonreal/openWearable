# -----------------------------------------------------------------------------
# Makefile for ankleOS
#
# J. Realmuto realmuto@uw.edu
# -----------------------------------------------------------------------------

TARGET = ankleOS_tui

# C Source Directories
CSRC_DIR = ./src
CINC_DIR = ./include
CLIB_DIR =

# Build Directories
TARGET_DIR = ./bin
BUILD_DIR = ./build

# Compiler
CC = gcc
CFLAGS = -Wall -g -O2 -mtune=cortex-a8 -march=armv7-a
LDFLAGS =

# Find source/target files
SOURCES = $(wildcard $(CSRC_DIR)/*.c)
OBJECTS = $(patsubst $(CSRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.c=.o))

# Link
$(TARGET): $(OBJECTS)
	@echo " Linking ..."
	@echo " $(SOURCES) "
	@mkdir -p $(TARGET_DIR)
	$(CC) $^ -o $(TARGET_DIR)/$(TARGET) $(LIB) -lm

$(BUILD_DIR)/%.o: $(CSRC_DIR)/%.c
	@echo " Building ... ";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILD_DIR)"; $(RM) -r $(BUILD_DIR)
	@echo " $(RM) -r $(TARGET_DIR)"; $(RM) -r $(TARGET_DIR)

.PHONY: clean
