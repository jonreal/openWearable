#!/bin/bash
#
# Set the clock on an air-gapped beaglebone from this machine's clock.
#
#   ./set_bone_time.sh <boneid>
#   ./set_bone_time.sh root@boneblack-test.local
#   ./set_bone_time.sh debian@192.168.7.2
#
# Time is pushed in UTC, so the bone's own timezone setting still applies.
# If the bone has a battery-backed RTC the time is also written to hardware.

set -e

BONE=$1

if [ -z "$BONE" ]; then
  echo "usage: $(basename $0) <boneid>   (e.g. root@boneblack-test.local)"
  exit 1
fi

# root doesn't need (and may not have) sudo
if [ "${BONE%%@*}" = "root" ]; then
  SUDO=
else
  SUDO=sudo
fi

NOW=$(date -u +'%Y-%m-%d %H:%M:%S')

ssh "$BONE" "$SUDO date -u -s '$NOW' && $SUDO hwclock -w 2>/dev/null; date"

echo "Host time (UTC): $NOW"
