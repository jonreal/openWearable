#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>

#include "pinconfig.h"
#include "control.h"
#include "tui.h"

tui_t *ptui;

/* ----------------------------------------------------------------------------
 * Function init_tui()
 * ------------------------------------------------------------------------- */
int init_tui(void)
{
  struct termios newt;
  struct sigaction action_ui;

  ptui = malloc(sizeof(tui_t));
  ptui->io_ready = 0;

  /* Terminal Settings */
  if(tcgetattr(0, &newt) != 0){
    printf("Failed to get terminal attributes\n.");
    return -1;
  }
  /* This is a hack. libpruio uses other terminal settings. I found the
   * setting that work for me by NOT including lipruio, then printing out
   * defualt terminal settings. Then setting these setting while including
   * libpruio.
   */
  newt.c_iflag = 1280;
  newt.c_oflag = 5;
  newt.c_cflag = 191;
  newt.c_lflag = 35387;
//  printf("c_iflag = %i.\n", newt.c_iflag);
//  printf("c_oflag = %i.\n", newt.c_oflag);
//  printf("c_cflag = %i.\n", newt.c_cflag);
//  printf("c_lflag = %i.\n", newt.c_lflag);
  if(tcsetattr(0, TCSANOW, &newt) != 0){
    printf("Failed to set terminal attributes\n.");
    return -1;
  }

  /* Set up action for SIGIO */
  action_ui.sa_handler = io_cb;
  sigemptyset(&action_ui.sa_mask);
  action_ui.sa_flags = 0;
  if(sigaction(SIGIO, &action_ui, NULL) == -1)
    printf("Error sigaction\n");

  /* Set up file descriptor */
  if(fcntl(0, F_SETOWN, getpid()) == -1){
    printf("F_SETOWN error.\n");
    return -1;
  }
  if(fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_ASYNC | O_NONBLOCK) == -1){
    printf("Error setting stdin fd flags.\n");
    return -1;
  }
  add_func_to_cleanup(&tui_cleanup);
  printf("TUI initialized.\n");
  return 0;
}

/* ----------------------------------------------------------------------------
 * Function io_cb()
 * ------------------------------------------------------------------------- */
static void io_cb(int sig)
{
  ptui->io_ready = 1;
}

/* ----------------------------------------------------------------------------
 * Function ui_menu_cb()
 * ------------------------------------------------------------------------- */
void tui_menu(void)
{
  printf("\n\n--------------------------\n");
  printf("Kp = %5.2f, Kd = %5.2f, pos_0 = %5.2f\n",
         get_Kp(), get_Kd(), RAD2DEG(get_pos_0()));

  printf("Menu: a - enter new Kp\n");
  printf("      s - enter new Kd\n");
  printf("      d - enter new pos_0 (deg)\n");
  printf("      f - open loop\n");
  printf("      e - exit\n");
  printf("--------------------------\n");
}

/* ----------------------------------------------------------------------------
 * Function tui()
 * ------------------------------------------------------------------------- */
int start_tui(void)
{
  char input_char;
  float input_float;

  tui_menu();
  fflush(stdout);
  while(1){
    if(ptui->io_ready){
      scanf(" %c", &input_char);
      if(input_char == 'e'){
        return 1;
      }
      else if(input_char == 'a'){
        printf("\t\tEnter new Kp: ");
        fflush(stdout);
        ptui->io_ready = 0;
        while(1){
          if(ptui->io_ready){
            scanf(" %f", &input_float);
            break;
          }
        }
        set_Kp(input_float);
        tui_menu();
        fflush(stdout);
      }
      else if(input_char == 's'){
        printf("\t\tEnter new Kd: ");
        fflush(stdout);
        ptui->io_ready = 0;
        while(1){
          if(ptui->io_ready){
            scanf(" %f", &input_float);
            break;
          }
        }
        set_Kd(input_float);
        tui_menu();
        fflush(stdout);
      }
      else if(input_char == 'd'){
        printf("\t\tEnter new pos_0 (deg): ");
        fflush(stdout);
        ptui->io_ready = 0;
        while(1){
          if(ptui->io_ready){
            scanf(" %f", &input_float);
            break;
          }
        }
        set_pos_0(DEG2RAD(input_float));
        tui_menu();
        fflush(stdout);
      }
      ptui->io_ready = 0;
    }
  }
}

int tui_cleanup(void)
{
  if(fcntl(0, F_SETOWN, NULL) == -1){
    printf("F_SETOWN error.\n");
    return -1;
  }
  if(fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_ASYNC & ~O_NONBLOCK) == -1){
    printf("Error setting stdin fd flags.\n");
    return -1;
  }
  free(ptui);
  ptui = NULL;

  printf("TUI cleaned up.\n");
  return 0;
}





//
//  scanf("%c", &user_input);
//
//  if (user_input == 'e'){
//    return 1;
//  }
//  else if (user_input == 'a'){
//    printf("\t\t enter new Kp: ");
//    while(1){
//      if(scanf("%f", &user_float_input) != -1)
//        break;
//    }
//    set_Kp(user_float_input);
//    ui_menu_cb();
//    return 0;
//  }
//  else if (user_input == 's'){
//    printf("\t\t enter new Kd: ");
//    while(1){
//      if(scanf("%f", &user_float_input) != -1)
//        break;
//    }
//    set_Kd(user_float_input);
//    ui_menu_cb();
//    return 0;
//  }
//  else if (user_input == 'd'){
//    printf("\t\t enter new pos_0: ");
//    while(1){
//      if(scanf("%f", &user_float_input) != -1)
//        break;
//    }
//    set_pos_0(DEG2RAD(user_float_input));
//    ui_menu_cb();
//    return 0;
//  }
//  else if (user_input == 'f'){
//    set_FB_cntrl(0);
//    set_FF_cntrl(1);
//    printf("\t\t Press 0 enter exit.");
//    while(1){
//      if(scanf("%f", &user_float_input) != -1)
//        break;
//    }
//    set_FF_cntrl(0);
//    set_pos_0(ANGLE_OFFSET);
//    ui_menu_cb();
//    set_FB_cntrl(1);
//    return 0;
//  }
//  else{
//    return 0;
//  }
//}


