# -----------------------------------------------------------------------------
# Makefile for ankleOS
#
# J. Realmuto realmuto@uw.edu
# -----------------------------------------------------------------------------

TARGET = main
PRU_TARGET_0 = pru0_sensor
PRU_TARGET_1 = pru1_control

# C Source Directories
CSRC_DIR = ./src
CINC_DIR = ./include
CLIB = -lprussdrv

# PRU_C Source Directories
PRU_CSRC_DIR = ./pru_src
PRU_CINC_DIR = -I./include/ -I/opt/source/pru-software-support-package/include/
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

# Filter out source/target files
SOURCES = $(filter-out $(CSRC_DIR)/$(TARGET).c, $(wildcard $(CSRC_DIR)/*.c))
OBJECTS = $(patsubst $(CSRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.c=.o))

# -----------------------------------------------------------------------------
# Application
# -----------------------------------------------------------------------------

# Link C
$(TARGET): $(OBJECTS) $(BUILD_DIR)/$(TARGET).o $(PRU_TARGET_0) $(PRU_TARGET_1)
	@echo "\n Linking application... \n";
	@mkdir -p $(TARGET_DIR)
	$(CC) $(OBJECTS) $(CSRC_DIR)/$(TARGET).c -I$(CINC_DIR) -o $(TARGET_DIR)/$(TARGET) $(CLIB) -lm

# Build Target
$(BUILD_DIR)/$(TARGET).o: $(CSRC_DIR)/$(TARGET).c
	@echo "\n Building application... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $< $(CLIB) -lm

# Build C dependencies
$(BUILD_DIR)/%.o: $(CSRC_DIR)/%.c
	@echo "\n Building c dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -c -o $@ $< $(CLIB) -lm

# -----------------------------------------------------------------------------
#  PRU 0
# -----------------------------------------------------------------------------

# Hex PRU
$(PRU_TARGET_0): $(PRU_TARGET_0).elf
	@echo "\n Running hexpru for pru0... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_HEX) $(PRU_CSRC_DIR)/bin.cmd $(BUILD_DIR)/$(PRU_TARGET_0).elf
	@mv text.bin $(TARGET_DIR)/$(PRU_TARGET_0)_text.bin
	@mv data.bin $(TARGET_DIR)/$(PRU_TARGET_0)_data.bin

# Link PRU
$(PRU_TARGET_0).elf: $(BUILD_DIR)/$(PRU_TARGET_0).o
	@echo "\n Linking pru0... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_CC) $(PRU_CFLAGS) -z $^ -llibc.a -m $(BUILD_DIR)/$(PRU_TARGET_0).map -o $(BUILD_DIR)/$(PRU_TARGET_0).elf $(PRU_CSRC_DIR)/AM335x_PRU.cmd

# Build PRU dependencies
$(BUILD_DIR)/$(PRU_TARGET_0).o: $(PRU_CSRC_DIR)/$(PRU_TARGET_0).c
	@echo "\n Building pru0... \n";
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# -----------------------------------------------------------------------------
#  PRU 1
# -----------------------------------------------------------------------------

# Hex PRU
$(PRU_TARGET_1): $(PRU_TARGET_1).elf
	@echo "\n Running hexpru for pru1... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_HEX) $(PRU_CSRC_DIR)/bin.cmd $(BUILD_DIR)/$(PRU_TARGET_1).elf
	@mv text.bin $(TARGET_DIR)/$(PRU_TARGET_1)_text.bin
	@mv data.bin $(TARGET_DIR)/$(PRU_TARGET_1)_data.bin

# Link PRU
$(PRU_TARGET_1).elf: $(BUILD_DIR)/$(PRU_TARGET_1).o
	@echo "\n Linking pru1... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_CC) $(PRU_CFLAGS) -z $^ -llibc.a -m $(BUILD_DIR)/$(PRU_TARGET_1).map -o $(BUILD_DIR)/$(PRU_TARGET_1).elf $(PRU_CSRC_DIR)/AM335x_PRU.cmd

# Build PRU dependencies
$(BUILD_DIR)/$(PRU_TARGET_1).o: $(PRU_CSRC_DIR)/$(PRU_TARGET_1).c
	@echo "\n Building pru1... \n";
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# -----------------------------------------------------------------------------
# Clean
# -----------------------------------------------------------------------------
clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILD_DIR)"; $(RM) -r $(BUILD_DIR)
	@echo " $(RM) -r $(TARGET_DIR)"; $(RM) -r $(TARGET_DIR)

.PHONY: clean
