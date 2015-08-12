# -----------------------------------------------------------------------------
# Makefile for ankleOS
#
# J. Realmuto realmuto@uw.edu
# -----------------------------------------------------------------------------

TARGET = main
PRU_TARGET = pru_main
PRU_ELF = $(PRU_TARGET).elf

# C Source Directories
CSRC_DIR = ./src
CINC_DIR = ./include
CLIB = -lprussdrv

# PRU_C Source Directories
PRU_CSRC_DIR = ./pru_src
PRU_CINC_DIR = -I./pru_include/ -I/opt/source/pru-software-support-package/include/
PRU_CLIB =

# Build Directories
TARGET_DIR = ./bin
BUILD_DIR = ./build

# Compiler
CC = gcc
CFLAGS = -std=gnu99 -Wall -g -O2 -mtune=cortex-a8 -march=armv7-a
LDFLAGS =

# PRU compiler
PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d
PRU_LDFLAGS =

# PRU hex
PRU_HEX = hexpru


# Find source/target files
SOURCES = $(wildcard $(CSRC_DIR)/*.c)
OBJECTS = $(patsubst $(CSRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.c=.o))

# Find source/target files pru
PRU_SOURCES = $(wildcard $(PRU_CSRC_DIR)/*.c)
PRU_OBJECTS = $(patsubst $(PRU_CSRC_DIR)/%,$(BUILD_DIR)/%,$(PRU_SOURCES:.c=.o))

# Link C
$(TARGET): $(OBJECTS) $(PRU_TARGET)
	@echo "\n Linking target... \n"
	@mkdir -p $(TARGET_DIR)
	$(CC) $(OBJECTS) -o $(TARGET_DIR)/$(TARGET) $(CLIB) -lm

# Build C
$(BUILD_DIR)/%.o: $(CSRC_DIR)/%.c
	@echo "\n Building target source dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $< $(CLIB) -lm

# Hex PRU
$(PRU_TARGET): $(PRU_ELF)
	@echo "\n Running hexpru... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_HEX) $(PRU_CSRC_DIR)/bin.cmd $(BUILD_DIR)/$(PRU_ELF)
	@mv text.bin $(TARGET_DIR)/$(PRU_TARGET)_text.bin
	@mv data.bin $(TARGET_DIR)/$(PRU_TARGET)_data.bin

# Link PRU
$(PRU_ELF): $(PRU_OBJECTS)
	@echo "\n Linking pru targets... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_CC) $(PRU_CFLAGS) -z $^ -llibc.a -m $(BUILD_DIR)/$(PRU_TARGET).map -o $(BUILD_DIR)/$(PRU_TARGET).elf $(PRU_CSRC_DIR)/AM335x_PRU.cmd

# Build PRU
$(BUILD_DIR)/%.o: $(PRU_CSRC_DIR)/%.c
	@echo "\n Building pru dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILD_DIR)"; $(RM) -r $(BUILD_DIR)
	@echo " $(RM) -r $(TARGET_DIR)"; $(RM) -r $(TARGET_DIR)

.PHONY: clean
