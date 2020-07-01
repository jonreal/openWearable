# ----

# Each app has following targets
TARGET = main
PRU0_TARGET = pru0_main
PRU1_TARGET = pru1_main

# openWearable path
OW_DIR = /root/openWearable

# C Source Directories
CSRC_DIR = $(OW_DIR)/library/src
CINC_DIR = $(OW_DIR)/library/include \
					 -I/usr/lib/ti/pru-software-support-package/include/am335x \
					 -I/usr/lib/ti/pru-software-support-package/include
CLIB = -lpthread
SRC_IGNORE =

# PRU_C Source Directories
PRU_CSRC_DIR = $(OW_DIR)/library/pru_src
PRU_CINC_DIR = -I$(OW_DIR)/library/include/ \
							 -I/usr/lib/ti/pru-software-support-package/include/am335x \
							 -I/usr/lib/ti/pru-software-support-package/include
PRU_CLIB =

# Build Directories
TARGET_DIR = ./bin
BUILD_DIR = $(OW_DIR)/apps/$(MAKECMDGOALS)/build

# ROS
ROS_DIR = $(OW_DIR)/ros
ROS_CPP_SRC = $(ROS_DIR)/rospub.cpp
ROS_OBJ = $(BUILD_DIR)/rospub.o
ROS_TARGET = $(BUILD_DIR)/libRosPub.so
ROS_LIB = -lrostime -lroscpp -lrosconsole -lroscpp_serialization
ROS_LIB_DIR = -L/opt/ros/kinetic/lib
ROS_INC_DIR = -I/opt/ros/kinetic/include

# Compiler
CC = gcc
CFLAGS = -std=gnu99 -Wall -g -O4 -mtune=cortex-a8 -march=armv7-a -D=am335x
LDFLAGS =

# PRU compiler
PRU_CC = clpru
PRU_CFLAGS = -q --c99 -v3 --define=am3359d -O4 -D=am335x -D=FIXMATH_NO_CACHE -D=PRU --hardware_mac=on --gen_func_subsections=on
PRU_LDFLAGS =

# Install stuff
INSTALL = install -m 755
INSTALLDIR = install -d -m 755
INSTALLNONEXEC = install -m 644

# ----
# CPU library sources (openWearable core)
CPU_SRC_PRE = $(filter-out $(CSRC_DIR)/$(TARGET).c, $(wildcard $(CSRC_DIR)/*.c))
CPU_SRC = $(filter-out $(CSRC_DIR)/$(SRC_IGNORE).c, $(CPU_SRC_PRE))
CPU_OBJ = $(patsubst $(CSRC_DIR)/%,$(BUILD_DIR)/%,$(CPU_SRC:.c=.o))

# App specific CPU sources
APP_SPEC_CPU_SRC = $(MAKECMDGOALS)/format.c $(MAKECMDGOALS)/uiloop.c $(MAKECMDGOALS)/cpuloop.c
APP_SPEC_CPU_OBJ = $(patsubst $(MAKECMDGOALS)/%,$(BUILD_DIR)/%,$(APP_SPEC_CPU_SRC:.c=.o))

# PRU library source (mostly drivers, ect)
PRU_SRC = $(filter-out $(PRU_CSRC_DIR)/$(PRU0_TARGET).c $(PRU_CSRC_DIR)/$(PRU1_TARGET).c, $(wildcard $(PRU_CSRC_DIR)/*.c))

PRU_OBJ = $(patsubst $(PRU_CSRC_DIR)/%,$(BUILD_DIR)/%,$(PRU_SRC:.c=.po))

# App specific pru source files
APP_SPEC_PRU_SRC = $(MAKECMDGOALS)/pruloop.c
APP_SPEC_PRU_OBJ = $(patsubst $(MAKECMDGOALS)/%,$(BUILD_DIR)/%,$(APP_SPEC_PRU_SRC:.c=.po))

APP_SPEC_INC = $(MAKECMDGOALS)/

# ----

# ---- Apps
#  For each app add the following recipes, but replacing template with dir name
#  of app

.PHONY : linker install \
					template clean-template \
					reflex clean-reflex \
					reflex-single clean-reflex-single


# --- Template
template : linker install
clean-template :
	@echo " Cleaning template...";
	@echo " $(RM) -r ./template/build"; $(RM) -r ./template/build
	@echo " $(RM) -r $(TARGET_DIR)/template"; $(RM) -r $(TARGET_DIR)/template
	@echo " $(RM) -r /lib/firmware/am335x-pru0-template-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-template-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-template-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-template-fw

# ---- CPU

# ros -- build shared lib
$(ROS_TARGET) : $(ROS_OBJ)
	@echo "\n Building shared ros lib ... \n";
	@mkdir -p $(BUILD_DIR)
	g++ -shared -o $@ $^ $(ROS_INC_DIR) $(ROS_LIB_DIR) $(ROS_LIB)

# ros -- build ros object
$(ROS_OBJ) : $(ROS_CPP_SRC)
	@echo "\n Building ros objects ... \n";
	@mkdir -p $(BUILD_DIR)
	g++ -c -Wall -fPIC -o $@ $^ $(ROS_INC_DIR) $(ROS_LIB_DIR) $(ROS_LIB)

# Linker -- Must define pru firmware suffix
linker : $(CPU_OBJ) $(APP_SPEC_CPU_OBJ)
	@echo "\n Linking cpu ... \n";
	@mkdir -p $(TARGET_DIR)
	$(CC) -Wl,-rpath,$(BUILD_DIR) -L$(BUILD_DIR) $(CFLAGS) $(CPU_OBJ) $(APP_SPEC_CPU_OBJ) $(CSRC_DIR)/$(TARGET).c -I$(CINC_DIR) -I$(APP_SPEC_INC) -I$(ROS_DIR) -o $(TARGET_DIR)/$(MAKECMDGOALS) $(CLIB) -lm -DFWSUFFIX=\"$(MAKECMDGOALS)\"
	#-lRosPub

# Build C dependencies
$(BUILD_DIR)%.o: $(CSRC_DIR)%.c
	@echo "\n Building core dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) -Wl,-rpath,$(BUILD_DIR) -L$(BUILD_DIR) $(CFLAGS) $(ROS_INC_DIR) -I$(ROS_DIR) -I$(CINC_DIR) -I./$(APP_SPEC_INC) -c -o $@ $< $(CLIB) -lm
	#-lRosPub

# Build app specific C dependencies
$(BUILD_DIR)/format.o : $(MAKECMDGOALS)/format.c
	@echo "\n Building app specific dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -I./$(APP_SPEC_INC) -c -o $@ $< $(CLIB) -lm

# Build app specific C dependencies
$(BUILD_DIR)/cpuloop.o : $(MAKECMDGOALS)/cpuloop.c
	@echo "\n Building app specific dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -I./$(APP_SPEC_INC) -c -o $@ $< $(CLIB) -lm

# Build app specific C dependencies
$(BUILD_DIR)/uiloop.o : $(MAKECMDGOALS)/uiloop.c
	@echo "\n Building app specific dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(CINC_DIR) -I$(ROS_DIR) -I./$(APP_SPEC_INC) -c -o $@ $< $(CLIB) -lm

# ---- PRU

# Install PRU
install : $(BUILD_DIR)/$(PRU0_TARGET).elf $(BUILD_DIR)/$(PRU1_TARGET).elf
	@echo "\n Installing pru firmware... \n";
	$(INSTALLDIR) /etc/modprobe.d
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU0_TARGET).elf /lib/firmware/am335x-pru0-$(MAKECMDGOALS)-fw
	$(INSTALLNONEXEC) $(BUILD_DIR)/$(PRU1_TARGET).elf /lib/firmware/am335x-pru1-$(MAKECMDGOALS)-fw

# Linker PRU0
$(BUILD_DIR)/$(PRU0_TARGET).elf : $(BUILD_DIR)/$(PRU0_TARGET).po $(PRU_OBJ) $(APP_SPEC_PRU_OBJ)
	@echo "\n Linking pru0... \n"
	$(PRU_CC) $(PRU_CFLAGS) -z --heap_size=1024 $^ -llibc.a -m $(MAKECMDGOALS)/build/$(PRU0_TARGET).map -o $(MAKECMDGOALS)/build/$(PRU0_TARGET).elf $(PRU_CSRC_DIR)/PRU0.cmd


# Linker PRU1
$(BUILD_DIR)/$(PRU1_TARGET).elf : $(BUILD_DIR)/$(PRU1_TARGET).po $(PRU_OBJ) $(APP_SPEC_PRU_OBJ)
	@echo "\n Linking pru1... \n"
	$(PRU_CC)  $(PRU_CFLAGS) -z --heap_size=1024 $^ -llibc.a -m $(MAKECMDGOALS)/build/$(PRU1_TARGET).map -o $(MAKECMDGOALS)/build/$(PRU1_TARGET).elf $(PRU_CSRC_DIR)/PRU1.cmd

# Build PRU-core
$(BUILD_DIR)%.po : $(PRU_CSRC_DIR)%.c
	@echo "\n Building pru dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -I$(APP_SPEC_INC) -c -fe $@ $<

# Build App specific PRU dependencies
$(BUILD_DIR)/pruloop.po : $(MAKECMDGOALS)/pruloop.c
	@echo "\n Building app specific pru dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(PRU_CC) $(PRU_CFLAGS) $(PRU_CINC_DIR) -I$(APP_SPEC_INC) -c -fe $@ $<

# --- DEBUG
debug :
	@echo "\n APP  = $(APP_NAME) ... \n";
	@echo "\n PRU-OBJ $(PRU_OBJ) ... \n";
	@echo "\n PRU-SRC $(PRU_SRC) ... \n";
	@echo "\n APP-PRU-OBJ $(APP_SPEC_PRU_OBJ) ... \n";
	@echo "\n APP-PRU-SRC $(APP_SPEC_PRU_SRC) ... \n";
	@echo "\n CPU-OBJ $(CPU_OBJ) ... \n";
	@echo "\n CPU-SRC $(CPU_SRC) ... \n";
	@echo "\n CPU-SRC-PRE $(CPU_SRC_PRE) ... \n";
	@echo "\n APP-CPU-OBJ $(APP_SPEC_CPU_OBJ) ... \n";
	@echo "\n APP-CPU-SRC $(APP_SPEC_CPU_SRC) ... \n";
	@echo "\n ROS_CPP_SRC $(ROS_CPP_SRC) ... \n";
	@echo "\n ROS_OBJ $(ROS_OBJ) ... \n";
	@echo "\n ROS_TARGET $(ROS_TARGET) ... \n";
