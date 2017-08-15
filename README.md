openWearable
===========

OpenWearable is an open source hardware/software project aimed at developing
tools for wearable robotics. The hardware is designed on top of the TI
Beaglebone Black (Linux beaglebone 3.8.13-bone79).

Currently in development and only supports powered ankle-foot prosthesis. More
to come. 

Connect with battery, plug to host CPU with USB connection (OSX):
~~~~
> screen /dev/tty.usb--
> ./openWearable/scripts/wifi-up
> ./openWearable/scripts/pru_enable
> exit
~~~

Unplug USB, then ssh to access user interface:
~~~

> ssh beaglebone.local
> cd openWearable/app
> make
> ./bin/main

---------------------
Welcome to AnklebotOS
---------------------
#
#Memory Allocation:
#	Parameter memory: 392 bytes.
#	Lookup table memory: 2000 bytes.
#	Data memory: 11928 bytes.
#
#Parameters:
#	Frq = 1000 (Hz)
#	Ticks = 200000
#	hs_delay = 40
#	u_bias = 1.0000
#	isProsLeft = 1
#	l_forceThrs = 500
#	l_d_forceThrs = 50
#	r_forceThrs = 500
#	r_d_forceThrs = 50
#
#Filter Coefficients (N = 1):
#	b[0] : 0.01850891	a[0] : 1.00000000
#	b[1] : 0.01850891	a[1] : -0.96299744
#

Press enter to start


TUI initialized.


---------------------------------------------------------------------
hs_delay = 40, u_bias = 1.00, ProsSide = L, FF enabled = 0, FFgain = 0.00
Menu: a - Enter new hs_delay
      s - Enter new u_bias
      d - Change prosthetic limb side
      f - Collect trial
      g - Save parameters
      h - Load parameters
      j - Load Feedforward lookup table
      k - Toggle feedforward control
      l - Enter new FFgain
      p - Test Feedforward
      q - Step Response
      e - exit
-----------------------------------------------------------------------

~~~~




-------------------------------------------------------------------------------

