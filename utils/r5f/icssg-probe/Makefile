# openWearable R5F <-> ICSSG shared-RAM access probe (diagnostic firmware).
# Tests whether the R5F can reach the PRU's ICSSG shared RAM (0x0b010000) -- the
# gating capability for the unified PRU+R5F memory model. See README.md.
CC      := arm-none-eabi-gcc
CFLAGS  := -mcpu=cortex-r5 -marm -mfloat-abi=soft -ffreestanding -nostdlib \
           -nostartfiles -O2 -Wall -I.
BUILD   := build
FW      := $(BUILD)/j7-r5f0-icssg-probe-fw.ow
PARK    := /lib/firmware/j7-r5f0-park-fw.ow

.PHONY: all install restore clean
all: $(FW)
$(FW): probe.c start.S probe.lds | $(BUILD)
	$(CC) $(CFLAGS) -T probe.lds start.S probe.c -o $@
$(BUILD):
	mkdir -p $(BUILD)

# Boot the probe on core0 by standing in for the park firmware (backed up).
# Then: reboot -> run the template app -> ./read.py -> `make restore` + reboot.
install: all
	[ -f $(PARK).bak ] || cp -a $(PARK) $(PARK).bak
	cp $(FW) $(PARK)
	@echo ">> probe installed as core0 boot fw. REBOOT, run template, then ./read.py"
restore:
	@[ -f $(PARK).bak ] && cp -a $(PARK).bak $(PARK) && rm -f $(PARK).bak && echo ">> park restored (reboot to apply)" || echo "no backup"
clean:
	$(RM) -r $(BUILD)
