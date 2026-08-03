#!/bin/sh
# test-ads131m08 runtime hardware setup -- the parts that can't live in the device
# tree (PWM enable + runtime-PM are sysfs actions). Run once after boot, before
# apps/bin/test-ads131m08. Idempotent. For auto-run at boot, install as a systemd
# service or add to /etc/rc.local.
#
#   CLKIN : EHRPWM2_A on P9.14 (pwmchip1) at ~8.192 MHz, 50%  -> ADS131M08 CLKIN
#   CS    : P9.28 = main_gpio1 line 11 muxed to GPIO -- the R5F drives it, because
#           the OMAP McSPI FORCE bit can't hold SPIEN across a frame on this IP
#   McSPI7: functional clock pinned on (the R5F mmaps 0x2170000; spidev left unused)
#
# The P9.14 mux is also set by the device tree; the devmem here is harmless
# belt-and-suspenders so this works even before the updated DTB is deployed.
set -e

# --- pin mux (CTRL_MMR pads) ------------------------------------------------
devmem2 0x0011C230 w 0x00050007 >/dev/null   # P9.28 -> GPIO output (mode 7)
devmem2 0x0011C178 w 0x0821400B >/dev/null   # P9.14 -> EHRPWM2_A  (mode 0xB)

# --- CLKIN: pwmchip1/pwm0 = EHRPWM2_A, 122 ns (~8.192 MHz), 50% -------------
[ -e /sys/class/pwm/pwmchip1/pwm0 ] || echo 0 > /sys/class/pwm/pwmchip1/export
echo 122 > /sys/class/pwm/pwmchip1/pwm0/period
echo 61  > /sys/class/pwm/pwmchip1/pwm0/duty_cycle
echo 1   > /sys/class/pwm/pwmchip1/pwm0/enable

# --- McSPI7: keep the functional clock on (co-resident; do not open the spidev) -
echo on > /sys/bus/platform/devices/2170000.spi/power/control

echo "test-ads131m08 HW ready: CLKIN=P9.14 ~8.192MHz, CS=P9.28 (gpio1.11), McSPI7 clock pinned"
