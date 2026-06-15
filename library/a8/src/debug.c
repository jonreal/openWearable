/* Copyright 2019 Jonathan Realmuto

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
=============================================================================*/

#include "debug.h"
#include <stdio.h>
#include "gpio.h"

#define GPIO_POWER_PIN 16
#define GPIO_POWER_CHIP 2

static int debug_chip;
static int debug_pin;

// preposessor macros
#define unlikely(x)	__builtin_expect (!!(x), 0)
#define likely(x)	__builtin_expect (!!(x), 1)

int DebugInit(void) {
  debug_chip = CPU_DEBUG_CHIP;
  debug_pin = CPU_DEBUG_PIN;

	if(unlikely(rc_gpio_init(debug_chip,debug_pin,GPIOHANDLE_REQUEST_OUTPUT))) {
		printf("ERROR in rc_motor_init, failed to set up gpio");
		return -1;
	}

//  // power rail for servos (for beaglebone blue only)
//  if(rc_gpio_init(GPIO_POWER_CHIP,GPIO_POWER_PIN, GPIOHANDLE_REQUEST_OUTPUT)==-1){
//    printf("ERROR in rc_servo_init, failed to set up power rail GPIO pin\n");
//    return -1;
//  }
//  if(rc_gpio_set_value(GPIO_POWER_CHIP,GPIO_POWER_PIN,1)==-1){
//    printf("ERROR in rc_servo_power_rail_en,failed to write to GPIO pin\n");
//    return -1;
//  }
//

  DebugPinLow();
  return 0;
}

void DebugPinHigh(void) {
	rc_gpio_set_value(debug_chip,debug_pin, 1);
}

void DebugPinLow(void) {
  rc_gpio_set_value(debug_chip,debug_pin, 0);
}

void DebugCleanup(void) {
  DebugPinLow();
//  rc_gpio_cleanup(GPIO_POWER_CHIP, GPIO_POWER_PIN);
  rc_gpio_cleanup(debug_chip,debug_pin);
}
