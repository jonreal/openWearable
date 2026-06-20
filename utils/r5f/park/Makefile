# openWearable R5F "park" firmware -- the default, stoppable boot stub the K3
# driver auto-boots; the A72 stops it (graceful-shutdown handshake) and loads OW
# control firmware at runtime. STANDALONE (not in library/, which apps build
# from). C bare-metal, arm-none-eabi-gcc, builds on the board; installs to
# /lib/firmware like the PRUs. Naming: j7-<proc>-<app>-fw.ow -> j7-r5f{0,1}-park-fw.ow
APP     := park
CORES   := 0 1
CC      := arm-none-eabi-gcc
CFLAGS  := -mcpu=cortex-r5 -marm -mfloat-abi=soft -ffreestanding -nostdlib \
           -nostartfiles -O2 -Wall -Wextra -I.
BUILD   := build
FWDIR   := /lib/firmware
INSTALL := install -m 644
SRC     := start.S park.c
FW      := $(foreach c,$(CORES),$(BUILD)/j7-r5f$(c)-$(APP)-fw.ow)

.PHONY: all install clean
all: $(FW)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/r5f%.lds: park.lds.in | $(BUILD)
	$(CC) -E -P -x c -DR5F_CORE=$* $< -o $@

$(BUILD)/j7-r5f%-$(APP)-fw.ow: $(SRC) $(BUILD)/r5f%.lds | $(BUILD)
	$(CC) $(CFLAGS) -DR5F_CORE=$* -T $(BUILD)/r5f$*.lds $(SRC) -o $@

install: all
	for c in $(CORES); do $(INSTALL) $(BUILD)/j7-r5f$$c-$(APP)-fw.ow $(FWDIR)/; done

clean:
	$(RM) -r $(BUILD)
	for c in $(CORES); do $(RM) $(FWDIR)/j7-r5f$$c-$(APP)-fw.ow; done
