# -----------------------------------------------------------------------------
# Makefile for ankleOS
#
# J. Realmuto realmuto@uw.edu
# -----------------------------------------------------------------------------


TARGET = main
PRU0_TARGET = pru0_main
PRU1_TARGET = pru1_main

# C Source Directories
CSRC_DIR = ../library/src
CINC_DIR = ../library/include -I/usr/lib/ti/pru-software-support-package/include/am335x -I/usr/lib/ti/pru-software-support-package/include
CLIB = -lpthread
SRC_IGNORE =

# PRU_C Source Directorie
#
PRU_CSRC_DIR = ../library/pru_src
PRU_CINC_DIR = -I../library/include/ -I/usr/lib/ti/pru-software-support-package/include/am335x -I/usr/lib/ti/pru-software-support-package/include
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
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4 -D=am335x -D=FIXMATH_NO_CACHE
PRU_LDFLAGS =

# Install stuff
INSTALL = install -m 755
INSTALLDIR = install -d -m 755
INSTALLNONEXEC = install -m 644

# CPU library sources (openWearable core)
CPU_SRC_PRE = $(filter-out $(CSRC_DIR)/$(TARGET).c, $(wildcard $(CSRC_DIR)/*.c))
CPU_SRC = $(filter-out $(CSRC_DIR)/$(SRC_IGNORE).c, $(CPU_SRC_PRE))
CPU_OBJ = $(patsubst $(CSRC_DIR)/%,$(BUILD_DIR)/%,$(CPU_SRC:.c=.o))

# App specific CPU sources
APP_SPEC_CPU_SRC = $(MAKECMDGOALS)/format.c $(MAKECMDGOALS)/tuiloop.c
APP_SPEC_CPU_OBJ = $(patsubst $(MAKECMDGOALS)/%,$(BUILD_DIR)/%,$(APP_SPEC_CPU_SRC:.c=.o))

# PRU library source (mostly drivers, ect)
PRU_SRC = $(filter-out $(PRU_CSRC_DIR)/$(PRU0_TARGET).c $(PRU_CSRC_DIR)/$(PRU1_TARGET).c, $(wildcard $(PRU_CSRC_DIR)/*.c))
PRU_OBJ = $(patsubst $(PRU_CSRC_DIR)/%,$(BUILD_DIR)/%,$(PRU_SRC:.c=.o))

# App specific pru source files
APP_SPEC_PRU_SRC = $(MAKECMDGOALS)/pruloop.c
APP_SPEC_PRU_OBJ = $(patsubst $(MAKECMDGOALS)/%,$(BUILD_DIR)/%,$(APP_SPEC_PRU_SRC:.c=.o))

# App specific include
APP_SPEC_INC = $(MAKECMDGOALS)/

GOALS =
APP =
# -----------------------------------------------------------------------------
# Application
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Clean
# -----------------------------------------------------------------------------

check4clean :
ifeq ($(MAKECMDGOALS),clean)
#	@echo " Cleaning...";
#	@echo " $(RM) -r $(BUILD_DIR)"; $(RM) -r $(BUILD_DIR)
#	@echo " $(RM) -r $(TARGET_DIR)"; $(RM) -r $(TARGET_DIR)
GOALS = cleanow
else
GOALS = $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf  $(BUILD_DIR)/$(TARGET).o $(APP_SPEC_CPU_OBJ) $(CPU_OBJ) linker install
APP = $(MAKECMDGOALS)
endif

cleanow:
	@echo " Cleaning...";
	@echo " $(RM) -r ./build"; $(RM) -r ./build
	@echo " $(RM) -r $(TARGET_DIR)"; $(RM) -r $(TARGET_DIR)

# Check if clean
$(MAKECMDGOALS): check4clean $(GOALS)
	@echo "";

# Link CPU
linker:
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) $(CPU_OBJ) $(APP_SPEC_CPU_OBJ) $(CSRC_DIR)/$(TARGET).c -I$(CINC_DIR) -I$(APP_SPEC_INC) -o $(TARGET_DIR)/$(MAKECMDGOALS) $(CLIB) -lm

# Link PRU0
$(BUILD_DIR)/$(PRU0_TARGET).elf: $(PRU_OBJ) $(APP_SPEC_PRU_OBJ) $(BUILD_DIR)/$(PRU0_TARGET).o
	@echo "\n Linking pru0... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_CC) $(PRU_CFLAGS) -z $^ -llibc.a -m $(BUILD_DIR)/$(PRU0_TARGET).map -o $(BUILD_DIR)/$(PRU0_TARGET).elf $(PRU_CSRC_DIR)/PRU0.cmd

# Link PRU1
$(BUILD_DIR)/$(PRU1_TARGET).elf: $(PRU_OBJ) $(APP_SPEC_PRU_OBJ) $(BUILD_DIR)/$(PRU1_TARGET).o
	@echo "\n Linking pru0... \n"
	@mkdir -p $(TARGET_DIR)
	$(PRU_CC) $(PRU_CFLAGS) -z $^ -llibc.a -m $(BUILD_DIR)/$(PRU1_TARGET).map -o $(BUILD_DIR)/$(PRU1_TARGET).elf $(PRU_CSRC_DIR)/PRU1.cmd

# Build C dependencies
$(BUILD_DIR)/%.o: $(CSRC_DIR)/%.c
	@echo "\n Building core dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -I./$(APP_SPEC_INC) -c -o $@ $< $(CLIB) -lm

# Build PRU dependencies
$(BUILD_DIR)/%.o: $(PRU_CSRC_DIR)/%.c
	@echo "\n Building pru dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -I$(APP_SPEC_INC) -c -fe $@ $<

# Build App specific PRU dependencies
$(BUILD_DIR)/pruloop.o: $(MAKECMDGOALS)/pruloop.c
	@echo "\n Building app specific pru dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -I$(APP_SPEC_INC) -c -fe $@ $<

# Build app specific C dependencies
$(BUILD_DIR)/format.o: $(MAKECMDGOALS)/format.c
	@echo "\n Building app specific dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -I./$(APP_SPEC_INC) -c -o $@ $< $(CLIB) -lm

# Build app specific C dependencies
$(BUILD_DIR)/tuiloop.o: $(MAKECMDGOALS)/tuiloop.c
	@echo "\n Building app specific dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -I./$(APP_SPEC_INC) -c -o $@ $< $(CLIB) -lm

# Copy firmware
install:
	@echo "\n Installing pru firmware... \n";
	$(INSTALLDIR) /etc/modprobe.d
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU0_TARGET).elf /lib/firmware/am335x-pru0-fw
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU1_TARGET).elf /lib/firmware/am335x-pru1-fw
#	$(INSTALLNONEXEC) pruss-blacklist.conf $(DESTDIR)/etc/modprobe.d/
	@echo "\n Install complete. \n"
