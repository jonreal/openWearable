/* Copyright 2018 Jonathan Realmuto

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

#include "gui.h"
#include "gnuplot_i.h"

int GuiInit(void) {
  printf("GUI initialized.\n");
  return 0;
}

int GuiLoop(const pru_mem_t* pru_mem) {
  gnuplot_ctrl* h = gnuplot_init();

  double d[10] = {0,};

  for (int i=0; i<100; i++) {
    d[i%10] = i;
    gnuplot_plot_x(h, d, 10,"");
    sleep(1);
  }

  gnuplot_close(h);
  return 0;
}

int GuiCleanup(void) {
  return 0;
}

