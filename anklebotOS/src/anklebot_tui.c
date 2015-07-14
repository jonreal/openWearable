#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <math.h>

#include "pinconfig.h"
#include "control.h"
#include "motor.h"
#include "spi.h"
#include "tui.h"

#define ANKLE_OS_v 1

int debug;

int main(int argc, char *argv[])
{
  const float frq_hz = 10;
  static FILE* f_log;

  /* Cmd line arguments */
  if ( (argc == 2) && (strcmp(argv[1],"-v") == 0) ){
    printf("DEBUG MODE.");
    debug = 1;
  }

  printf("\n---------------------\n");
  printf("Welcome to AnkleOS\n");
  printf("---------------------\n");

  if(init_control(frq_hz, f_log) != 0){
    perror("controller initialization failed.");
    return -1;
  }

  if(start_control() != 0){
    perror("controller start failed.");
    return -1;
  }

  ui_menu_cb();
  while(1){
    pause();
    ui_input();
  }
}

