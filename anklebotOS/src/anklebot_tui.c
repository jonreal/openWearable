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

#define ANKLE_OS_v 1

const int debug = 1;

int main(int argc, char *argv[])
{
  const float frq_hz = 20;
  static FILE* f_log;

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

