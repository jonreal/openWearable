# -----------------------------------------------------------------------------
# Makefile for ankleOS
#
# J. Realmuto realmuto@uw.edu
# -----------------------------------------------------------------------------

TARGET = main
PRU0_TARGET = pru0_main
PRU1_TARGET = pru1_main
SRC_IGNORE = testFilters

# C Source Directories
CSRC_DIR = ./src
CINC_DIR = ./include -I/usr/lib/ti/pru-software-support-package/include/am335x -I/usr/lib/ti/pru-software-support-package/include
CLIB = -lpthread

# PRU_C Source Directories
PRU_CSRC_DIR = ./pru_src
PRU_CINC_DIR = -I./include/ -I/usr/lib/ti/pru-software-support-package/include/am335x -I/usr/lib/ti/pru-software-support-package/include
PRU_CLIB =

# Build Directories
TARGET_DIR = ./bin
BUILD_DIR = ./build

# Compiler
CC = gcc
CFLAGS = -std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a -D=am335x
LDFLAGS =

# PRU compiler
PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4 -D=am335x
PRU_LDFLAGS =

# Install stuff
INSTALL = install -m 755
INSTALLDIR = install -d -m 755
INSTALLNONEXEC = install -m 644

# Filter out source/target files and other targets
SOURCES_PRE = $(filter-out $(CSRC_DIR)/$(TARGET).c, $(wildcard $(CSRC_DIR)/*.c))
SOURCES = $(filter-out $(CSRC_DIR)/$(SRC_IGNORE).c, $(SOURCES_PRE))
OBJECTS = $(patsubst $(CSRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.c=.o))

PRU0_SOURCE = $(filter-out $(PRU_CSRC_DIR)/$(PRU0_TARGET).c $(PRU_CSRC_DIR)/$(PRU1_TARGET).c, $(wildcard $(PRU_CSRC_DIR)/*.c))
PRU0_OBJECTS = $(patsubst $(PRU_CSRC_DIR)/%,$(BUILD_DIR)/%,$(PRU0_SOURCE:.c=.o))

PRU1_SOURCE = $(filter-out $(PRU_CSRC_DIR)/$(PRU0_TARGET).c $(PRU_CSRC_DIR)/$(PRU1_TARGET).c, $(wildcard $(PRU_CSRC_DIR)/*.c))
PRU1_OBJECTS = $(patsubst $(PRU_CSRC_DIR)/%,$(BUILD_DIR)/%,$(PRU1_SOURCES:.c=.o))

# -----------------------------------------------------------------------------
# Application
# -----------------------------------------------------------------------------

# Link C
$(TARGET): $(OBJECTS) $(BUILD_DIR)/$(TARGET).o $(PRU0_OBJECTS) $(PRU1_OBJECTS) $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf
	@echo "\n Linking application... \n";
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) $(OBJECTS) $(CSRC_DIR)/$(TARGET).c -I$(CINC_DIR) -o $(TARGET_DIR)/$(TARGET) $(CLIB) -lm

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
#  PRU
# -----------------------------------------------------------------------------

# Link PRU
$(BUILD_DIR)/$(PRU0_TARGET).elf: $(PRU0_OBJECTS) $(BUILD_DIR)/$(PRU0_TARGET).o
	@echo "\n Linking pru0... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_CC) $(PRU_CFLAGS) -z $^ -llibc.a -m $(BUILD_DIR)/$(PRU0_TARGET).map -o $(BUILD_DIR)/$(PRU0_TARGET).elf $(PRU_CSRC_DIR)/PRU0.cmd

# Link PRU
$(BUILD_DIR)/$(PRU1_TARGET).elf: $(PRU0_OBJECTS) $(BUILD_DIR)/$(PRU1_TARGET).o
	@echo "\n Linking pru1... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_CC) $(PRU_CFLAGS) -z $^ -llibc.a -m $(BUILD_DIR)/$(PRU1_TARGET).map -o $(BUILD_DIR)/$(PRU1_TARGET).elf $(PRU_CSRC_DIR)/PRU1.cmd

# Build PRU dependencies
$(BUILD_DIR)/%.o: $(PRU_CSRC_DIR)/%.c
	@echo "\n Building pru dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -c -fe $@ $<

# Copy firmware
install: $(TARGET)
	@echo "\n Installing pru firmware... \n";
	$(INSTALLDIR) /etc/modprobe.d
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU0_TARGET).elf /lib/firmware/am335x-pru0-fw
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU1_TARGET).elf /lib/firmware/am335x-pru1-fw
	$(INSTALLNONEXEC) pruss-blacklist.conf $(DESTDIR)/etc/modprobe.d/
	@echo "\n Install complete. \n"

# -----------------------------------------------------------------------------
# Clean
# -----------------------------------------------------------------------------
clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILD_DIR)"; $(RM) -r $(BUILD_DIR)
	@echo " $(RM) -r $(TARGET_DIR)"; $(RM) -r $(TARGET_DIR)


.PHONY: clean


# -----------------------------------------------------------------------------
# Test Filters
# -----------------------------------------------------------------------------

# Link C
filtTest:
	@echo "\n Linking application... \n";
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -o $(TARGET_DIR)/filtTest $(PRU_CSRC_DIR)/fix16.c $(PRU_CSRC_DIR)/filter.c $(CSRC_DIR)/testFilters.c -I$(CINC_DIR) $(CLIB) -lm


