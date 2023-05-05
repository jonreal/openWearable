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
#ROS_DIR = $(OW_DIR)/ros
#ROS_CPP_SRC = $(ROS_DIR)/rospub.cpp
#ROS_OBJ = $(BUILD_DIR)/rospub.o
#ROS_TARGET = $(BUILD_DIR)/libRosPub.so
#ROS_LIB = -lrostime -lroscpp -lrosconsole -lroscpp_serialization
#ROS_LIB_DIR = -L/opt/ros/kinetic/lib
#ROS_INC_DIR = -I/opt/ros/kinetic/include

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


# -----------------------------------------------------------------------------------------------------------------
# --- Template
# ---
template : linker install
clean-template :
	@echo " Cleaning template...";
	@echo " $(RM) -r ./template/build"; $(RM) -r ./template/build
	@echo " $(RM) -r $(TARGET_DIR)/template"; $(RM) -r $(TARGET_DIR)/template
	@echo " $(RM) -r /lib/firmware/am335x-pru0-template-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-template-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-template-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-template-fw
# -----------------------------------------------------------------------------------------------------------------

# -----------------------------------------------------------------------------------------------------------------
# --- sysID-dual-actuators
# ---
sysID-dual-actuators : linker install
clean-sysID-dual-actuators :
	@echo " Cleaning sysID-dual-actuators...";
	@echo " $(RM) -r ./sysID-dual-actuators/build"; $(RM) -r ./sysID-dual-actuators/build
	@echo " $(RM) -r $(TARGET_DIR)/sysID-dual-actuators"; $(RM) -r $(TARGET_DIR)/sysID-dual-actuators
	@echo " $(RM) -r /lib/firmware/am335x-pru0-sysID-dual-actuators-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-sysID-dual-actuators-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-sysID-dual-actuators-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-sysID-dual-actuators-fw
# -----------------------------------------------------------------------------------------------------------------


# ---
plotter-example : linker install
clean-plotter-example :
	@echo " Cleaning plotter-example...";
	@echo " $(RM) -r ./plotter-example/build"; $(RM) -r ./plotter-example/build
	@echo " $(RM) -r $(TARGET_DIR)/plotter-example"; $(RM) -r $(TARGET_DIR)/plotter-example
	@echo " $(RM) -r /lib/firmware/am335x-pru0-plotter-example-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-plotter-example-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-plotter-example-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-plotter-example-fw
# -----------------------------------------------------------------------------------------------------------------

# ---
pam-cycle-test : linker install
clean-pam-cycle-test :
	@echo " Cleaning pam-cycle-test...";
	@echo " $(RM) -r ./pam-cycle-test/build"; $(RM) -r ./pam-cycle-test/build
	@echo " $(RM) -r $(TARGET_DIR)/pam-cycle-test"; $(RM) -r $(TARGET_DIR)/pam-cycle-test
	@echo " $(RM) -r /lib/firmware/am335x-pru0-pam-cycle-test-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-pam-cycle-test-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-pam-cycle-test-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-pam-cycle-test-fw
# -----------------------------------------------------------------------------------------------------------------

# ---
udptest : linker install
clean-udptest :
	@echo " Cleaning udptest...";
	@echo " $(RM) -r ./udptest/build"; $(RM) -r ./udptest/build
	@echo " $(RM) -r $(TARGET_DIR)/udptest"; $(RM) -r $(TARGET_DIR)/udptest
	@echo " $(RM) -r /lib/firmware/am335x-pru0-udptest-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-udptest-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-udptest-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-udptest-fw

# ---
test-pwm : linker install
clean-test-pwm :
	@echo " Cleaning test-pwm...";
	@echo " $(RM) -r ./test-pwm/build"; $(RM) -r ./test-pwm/build
	@echo " $(RM) -r $(TARGET_DIR)/test-pwm"; $(RM) -r $(TARGET_DIR)/test-pwm
	@echo " $(RM) -r /lib/firmware/am335x-pru0-test-pwm-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-test-pwm-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-test-pwm-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-test-pwm-fw

# ---
test-maxon : linker install
clean-test-maxon :
	@echo " Cleaning test-maxon...";
	@echo " $(RM) -r ./test-maxon/build"; $(RM) -r ./test-maxon/build
	@echo " $(RM) -r $(TARGET_DIR)/test-maxon"; $(RM) -r $(TARGET_DIR)/test-maxon
	@echo " $(RM) -r /lib/firmware/am335x-pru0-test-maxon-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-test-maxon-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-test-maxon-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-test-maxon-fw


# ---
pam-step-response : linker install
clean-pam-step-response :
	@echo " Cleaning pam-step-response...";
	@echo " $(RM) -r ./pam-step-response/build"; $(RM) -r ./pam-step-response/build
	@echo " $(RM) -r $(TARGET_DIR)/pam-step-response"; $(RM) -r $(TARGET_DIR)/pam-step-response
	@echo " $(RM) -r /lib/firmware/am335x-pru0-pam-step-response-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-pam-step-response-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-pam-step-response-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-pam-step-response-fw

# ---
close-all-valves : linker install
clean-close-all-valves :
	@echo " Cleaning close-all-valves...";
	@echo " $(RM) -r ./close-all-valves/build"; $(RM) -r ./close-all-valves/build
	@echo " $(RM) -r $(TARGET_DIR)/close-all-valves"; $(RM) -r $(TARGET_DIR)/close-all-valves
	@echo " $(RM) -r /lib/firmware/am335x-pru0-close-all-valves-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-close-all-valves-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-close-all-valves-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-close-all-valves-fw

# ---
maxon-freq-sweep : linker install
clean-maxon-freq-sweep :
	@echo " Cleaning maxon-freq-sweep...";
	@echo " $(RM) -r ./maxon-freq-sweep/build"; $(RM) -r ./maxon-freq-sweep/build
	@echo " $(RM) -r $(TARGET_DIR)/maxon-freq-sweep"; $(RM) -r $(TARGET_DIR)/maxon-freq-sweep
	@echo " $(RM) -r /lib/firmware/am335x-pru0-maxon-freq-sweep-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-maxon-freq-sweep-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-maxon-freq-sweep-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-maxon-freq-sweep-fw

# ---
antagonist-freq-sweep : linker install
clean-antagonist-freq-sweep :
	@echo " Cleaning antagonist-freq-sweep...";
	@echo " $(RM) -r ./antagonist-freq-sweep/build"; $(RM) -r ./antagonist-freq-sweep/build
	@echo " $(RM) -r $(TARGET_DIR)/antagonist-freq-sweep"; $(RM) -r $(TARGET_DIR)/antagonist-freq-sweep
	@echo " $(RM) -r /lib/firmware/am335x-pru0-antagonist-freq-sweep-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-antagonist-freq-sweep-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-antagonist-freq-sweep-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-antagonist-freq-sweep-fw

# ---
reflex : linker install
clean-reflex :
	@echo " Cleaning reflex...";
	@echo " $(RM) -r ./reflex/build"; $(RM) -r ./reflex/build
	@echo " $(RM) -r $(TARGET_DIR)/reflex"; $(RM) -r $(TARGET_DIR)/reflex
	@echo " $(RM) -r /lib/firmware/am335x-pru0-reflex-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-reflex-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-reflex-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-reflex-fw

# ---
reflex-tracking : linker install
clean-reflex-tracking :
	@echo " Cleaning reflex-tracking...";
	@echo " $(RM) -r ./reflex-tracking/build"; $(RM) -r ./reflex-tracking/build
	@echo " $(RM) -r $(TARGET_DIR)/reflex-tracking"; $(RM) -r $(TARGET_DIR)/reflex-tracking
	@echo " $(RM) -r /lib/firmware/am335x-pru0-reflex-tracking-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-reflex-tracking-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-reflex-tracking-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-reflex-tracking-fw

# ---
tare-encoder : linker install
clean-tare-encoder :
	@echo " Cleaning tare-encoder...";
	@echo " $(RM) -r ./tare-encoder/build"; $(RM) -r ./tare-encoder/build
	@echo " $(RM) -r $(TARGET_DIR)/tare-encoder"; $(RM) -r $(TARGET_DIR)/tare-encoder
	@echo " $(RM) -r /lib/firmware/am335x-pru0-tare-encoder-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-tare-encoder-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-tare-encoder-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-tare-encoder-fw

# ---
thumbsup-triggered : linker install
clean-thumbsup-triggered :
	@echo " Cleaning thumbsup-triggered...";
	@echo " $(RM) -r ./thumbsup-triggered/build"; $(RM) -r ./thumbsup-triggered/build
	@echo " $(RM) -r $(TARGET_DIR)/thumbsup-triggered"; $(RM) -r $(TARGET_DIR)/thumbsup-triggered
	@echo " $(RM) -r /lib/firmware/am335x-pru0-thumbsup-triggered-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-thumbsup-triggered-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-thumbsup-triggered-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-thumbsup-triggered-fw

# ---
thumbsup-gas : linker install
clean-thumbsup-gas :
	@echo " Cleaning thumbsup-gas...";
	@echo " $(RM) -r ./thumbsup-gas/build"; $(RM) -r ./thumbsup-gas/build
	@echo " $(RM) -r $(TARGET_DIR)/thumbsup-gas"; $(RM) -r $(TARGET_DIR)/thumbsup-gas
	@echo " $(RM) -r /lib/firmware/am335x-pru0-thumbsup-gas-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-thumbsup-gas-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-thumbsup-gas-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-thumbsup-gas-fw

# ---
thumbsup-assist : linker install
clean-thumbsup-assist :
	@echo " Cleaning thumbsup-assist...";
	@echo " $(RM) -r ./thumbsup-assist/build"; $(RM) -r ./thumbsup-assist/build
	@echo " $(RM) -r $(TARGET_DIR)/thumbsup-assist"; $(RM) -r $(TARGET_DIR)/thumbsup-assist
	@echo " $(RM) -r /lib/firmware/am335x-pru0-thumbsup-assist-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-thumbsup-assist-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-thumbsup-assist-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-thumbsup-assist-fw


# ---
pamtest : linker install
clean-pamtest :
	@echo " Cleaning pamtest...";
	@echo " $(RM) -r ./pamtest/build"; $(RM) -r ./pamtest/build
	@echo " $(RM) -r $(TARGET_DIR)/pamtest"; $(RM) -r $(TARGET_DIR)/pamtest
	@echo " $(RM) -r /lib/firmware/am335x-pru0-pamtest-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-pamtest-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-pamtest-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-pamtest-fw

# ---
pam-starecase : linker install
clean-pam-starecase :
	@echo " Cleaning pam-starecase...";
	@echo " $(RM) -r ./pam-starecase/build"; $(RM) -r ./pam-starecase/build
	@echo " $(RM) -r $(TARGET_DIR)/pam-starecase"; $(RM) -r $(TARGET_DIR)/pam-starecase
	@echo " $(RM) -r /lib/firmware/am335x-pru0-pam-starecase-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-pam-starecase-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-pam-starecase-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-pam-starecase-fw

# ---
haptic-tracking : linker install
clean-haptic-tracking :
	@echo " Cleaning haptic-tracking...";
	@echo " $(RM) -r ./haptic-tracking/build"; $(RM) -r ./haptic-tracking/build
	@echo " $(RM) -r $(TARGET_DIR)/haptic-tracking"; $(RM) -r $(TARGET_DIR)/haptic-tracking
	@echo " $(RM) -r /lib/firmware/am335x-pru0-haptic-tracking-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-haptic-tracking-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-haptic-tracking-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-haptic-tracking-fw

# ---
haptic-eval :  linker install
clean-haptic-eval :
	@echo " Cleaning haptic-eval...";
	@echo " $(RM) -r ./haptic-eval/build"; $(RM) -r ./haptic-eval/build
	@echo " $(RM) -r $(TARGET_DIR)/haptic-eval"; $(RM) -r $(TARGET_DIR)/haptic-eval
	@echo " $(RM) -r /lib/firmware/am335x-pru0-haptic-eval-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-haptic-eval-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-haptic-eval-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-haptic-eval-fw

# ---
haptic-experiment : linker install
clean-haptic-experiment :
	@echo " Cleaning haptic-experiment...";
	@echo " $(RM) -r ./haptic-experiment/build"; $(RM) -r ./haptic-experiment/build
	@echo " $(RM) -r $(TARGET_DIR)/haptic-experiment"; $(RM) -r $(TARGET_DIR)/haptic-experiment
	@echo " $(RM) -r /lib/firmware/am335x-pru0-haptic-experiment-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-haptic-experiment-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-haptic-experiment-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-haptic-experiment-fw

# ---
imutest : linker install
clean-imutest :
	@echo " Cleaning imutest...";
	@echo " $(RM) -r ./imutest/build"; $(RM) -r ./imutest/build
	@echo " $(RM) -r $(TARGET_DIR)/imutest"; $(RM) -r $(TARGET_DIR)/imutest
	@echo " $(RM) -r /lib/firmware/am335x-pru0-imutest-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-imutest-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-imutest-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-imutest-fw

# ---
buttontest : linker install
clean-buttontest :
	@echo " Cleaning buttontest...";
	@echo " $(RM) -r ./buttontest/build"; $(RM) -r ./buttontest/build
	@echo " $(RM) -r $(TARGET_DIR)/buttontest"; $(RM) -r $(TARGET_DIR)/buttontest
	@echo " $(RM) -r /lib/firmware/am335x-pru0-buttontest-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru0-buttontest-fw
	@echo " $(RM) -r /lib/firmware/am335x-pru1-buttontest-fw"; $(RM) -r $(RM) -r /lib/firmware/am335x-pru1-buttontest-fw

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
	$(CC) -Wl,-rpath,$(BUILD_DIR) -L$(BUILD_DIR) $(CFLAGS) $(CPU_OBJ) $(APP_SPEC_CPU_OBJ) $(CSRC_DIR)/$(TARGET).c -I$(CINC_DIR) -I$(APP_SPEC_INC) -o $(TARGET_DIR)/$(MAKECMDGOALS) $(CLIB) -lm -DFWSUFFIX=\"$(MAKECMDGOALS)\"

# Build C dependencies
$(BUILD_DIR)%.o: $(CSRC_DIR)%.c
	@echo "\n Building core dependencies... \n";
	@mkdir -p $(BUILD_DIR)
	$(CC) -Wl,-rpath,$(BUILD_DIR) -L$(BUILD_DIR) $(CFLAGS) -I$(CINC_DIR) -I./$(APP_SPEC_INC) -c -o $@ $< $(CLIB) -lm

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
	$(CC) $(CFLAGS) -I$(CINC_DIR) -I./$(APP_SPEC_INC) -c -o $@ $< $(CLIB) -lm

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
