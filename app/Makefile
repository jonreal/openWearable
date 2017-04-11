# -----------------------------------------------------------------------------
# Makefile for ankleOS
#
# J. Realmuto realmuto@uw.edu
# -----------------------------------------------------------------------------

TARGET = main
PRU0_TARGET = pru0_main
PRU1_TARGET = pru1_main

# C Source Directories
CSRC_DIR = ./src
CINC_DIR = ./include
CLIB = -lprussdrv -lpthread

# PRU_C Source Directories
PRU_CSRC_DIR = ./pru_src
PRU_CINC_DIR = -I./include/ -I/opt/source/pru-software-support-package/include -I/opt/source/pru-software-support-package/include/am335x
PRU_CLIB =

# Build Directories
TARGET_DIR = ./bin
BUILD_DIR = ./build

# Compiler
CC = gcc
CFLAGS = -std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a
LDFLAGS =

# PRU compiler
PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4
PRU_LDFLAGS =

# PRU hex
PRU_HEX = hexpru

# Filter out source/target files
SOURCES = $(filter-out $(CSRC_DIR)/$(TARGET).c, $(wildcard $(CSRC_DIR)/*.c))
OBJECTS = $(patsubst $(CSRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.c=.o))

PRU0_SOURCE = $(filter-out $(PRU_CSRC_DIR)/$(PRU0_TARGET).c $(PRU_CSRC_DIR)/$(PRU1_TARGET).c, $(wildcard $(PRU_CSRC_DIR)/*.c))
PRU0_OBJECTS = $(patsubst $(PRU_CSRC_DIR)/%,$(BUILD_DIR)/%,$(PRU0_SOURCE:.c=.o))

PRU1_SOURCE = $(filter-out $(PRU_CSRC_DIR)/$(PRU0_TARGET).c $(PRU_CSRC_DIR)/$(PRU1_TARGET).c, $(wildcard $(PRU_CSRC_DIR)/*.c))
PRU1_OBJECTS = $(patsubst $(PRU_CSRC_DIR)/%,$(BUILD_DIR)/%,$(PRU1_SOURCES:.c=.o))
# -----------------------------------------------------------------------------
# Application
# -----------------------------------------------------------------------------

# Link C
$(TARGET): $(OBJECTS) $(BUILD_DIR)/$(TARGET).o $(PRU0_OBJECTS) $(PRU1_OBJECTS) $(PRU0_TARGET) $(PRU1_TARGET)
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
#  PRU0
# -----------------------------------------------------------------------------

# Hex PRU0
$(PRU0_TARGET): $(PRU0_TARGET).elf
	@echo "\n Running hexpru for pru0... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_HEX) $(PRU_CSRC_DIR)/bin.cmd $(BUILD_DIR)/$(PRU0_TARGET).elf
	@mv text.bin $(TARGET_DIR)/$(PRU0_TARGET)_text.bin
	@mv data.bin $(TARGET_DIR)/$(PRU0_TARGET)_data.bin

# Link PRU0
$(PRU0_TARGET).elf: $(PRU0_OBJECTS) $(BUILD_DIR)/$(PRU0_TARGET).o
	@echo "\n Linking pru0... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_CC) $(PRU_CFLAGS) -z $^ -llibc.a -m $(BUILD_DIR)/$(PRU0_TARGET).map -o $(BUILD_DIR)/$(PRU0_TARGET).elf $(PRU_CSRC_DIR)/PRU0.cmd

# Build PRU0 dependencies
$(BUILD_DIR)/%.o: $(PRU_CSRC_DIR)/%.c
	@echo "\n Building pru0 dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# -----------------------------------------------------------------------------
#  PRU1
# -----------------------------------------------------------------------------

# Hex PRU
$(PRU1_TARGET): $(PRU1_TARGET).elf
	@echo "\n Running hexpru for pru1... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_HEX) $(PRU_CSRC_DIR)/bin.cmd $(BUILD_DIR)/$(PRU1_TARGET).elf
	@mv text.bin $(TARGET_DIR)/$(PRU1_TARGET)_text.bin
	@mv data.bin $(TARGET_DIR)/$(PRU1_TARGET)_data.bin

# Link PRU
$(PRU1_TARGET).elf: $(PRU0_OBJECTS) $(BUILD_DIR)/$(PRU1_TARGET).o
	@echo "\n Linking pru1... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_CC) $(PRU_CFLAGS) -z $^ -llibc.a -m $(BUILD_DIR)/$(PRU1_TARGET).map -o $(BUILD_DIR)/$(PRU1_TARGET).elf $(PRU_CSRC_DIR)/PRU1.cmd

# Build PRU0 dependencies
$(BUILD_DIR)/%.o: $(PRU_CSRC_DIR)/%.c
	@echo "\n Building pru1 dependencies... \n";
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
