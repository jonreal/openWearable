#!/bin/sh
# test-ads131m08 runtime setup -- now minimal. The ads131 driver (with epwm_j721e +
# gpio_j721e) generates CLKIN on EHRPWM2, drives the CS GPIO, and remuxes the CS pad
# (P9.28 -> GPIO) itself, all in R5F firmware; the device tree muxes P9.14 ->
# EHRPWM2_A. The only thing the R5F can't do is enable the J721E peripheral clocks,
# so pin those here (co-resident: Linux holds the clock, the R5F drives the regs).
# Run once after boot, before apps/bin/test-ads131m08. Idempotent.
set -e

echo on > /sys/bus/platform/devices/3020000.pwm/power/control   # EHRPWM2 (CLKIN) clock
echo on > /sys/bus/platform/devices/2170000.spi/power/control   # McSPI7 (SPI) clock

echo "test-ads131m08: EHRPWM2 + McSPI7 clocks pinned (CLKIN/CS/pad-mux are in firmware)"
