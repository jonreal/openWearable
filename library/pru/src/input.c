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

#include "input.h"
#include <stdlib.h>

button_t* InputButtonInit(uint8_t pin, uint8_t debounce_cnt) {
  button_t* button = malloc(sizeof(button_t));
  button->pin = pin;
  button->debounce_cnt = debounce_cnt;

  button->cnt = 0;
  button->state = 0;
  button->prev_state = 0;
  return button;
}

void InputButtonUpdate(button_t* button) {

  uint8_t temp = ((__R31 & (1 << button->pin)) == (1 << button->pin));

  // check if transition, reset cnt
  if (temp != button->prev_state) {
    button->cnt == 0;
  } else {
    button->cnt++;
  }

  if (button->cnt > button->debounce_cnt)
    button->state = temp;

  button->prev_state = temp;
}

uint8_t InputButtonGetState(button_t* button) {
  return button->state;
}

void InputButtonCleanup(button_t* button) {
  free(button);
}

