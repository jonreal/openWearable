/* Copyright 2017 Jonathan Realmuto

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

#ifndef _TUI_H__
#define _TUI_H_


typedef struct{
  volatile sig_atomic_t io_ready;
  char input_char[2];
  char input_float[10];
} tui_t;


/* prototypes ------------------------------------------------------------- */
int init_tui(void);
void io_cb(int sig);
void tui_menu(void);
int start_tui(void);
int tui_cleanup(void);
void logData(void);
#endif
