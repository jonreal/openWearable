#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>

#include "pinconfig.h"
#include "control.h"
#include "tui.h"


/* ----------------------------------------------------------------------------
 * Function init_tui()
 * ------------------------------------------------------------------------- */
int init_tui(void)
{
  struct termios newt;

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
  return 0;
}
/* ----------------------------------------------------------------------------
 * Function ui_menu_cb()
 * ------------------------------------------------------------------------- */
void ui_menu_cb(void)
{
  printf("\n\n--------------------------\n");
  printf("Kp = %f, Kd = %f, pos_0 = %f\n",
         get_Kp(), get_Kd(), RAD2DEG(get_pos_0()));

  printf("Menu: a - enter new Kp\n");
  printf("      s - enter new Kd\n");
  printf("      d - enter new pos_0 (deg)\n");
  printf("      f - open loop\n");
  printf("      e - exit\n");
  printf("--------------------------\n");
}

/* ----------------------------------------------------------------------------
 * Function ui_input()
 * ------------------------------------------------------------------------- */
void ui_input(void)
{
  char user_input = 0;
  float user_float_input = 0.0;

  scanf("%c", &user_input);

  if (user_input == 'e'){
    control_cleanup(0);
  }
  else if (user_input == 'a'){
    printf("\t\t enter new Kp: ");
    while(1){
      pause();
      if(scanf("%f", &user_float_input) != -1)
        break;
    }
    set_Kp(user_float_input);
    ui_menu_cb();
  }
  else if (user_input == 's'){
    printf("\t\t enter new Kd: ");
    while(1){
      pause();
      if(scanf("%f", &user_float_input) != -1)
        break;
    }
    set_Kd(user_float_input);
    ui_menu_cb();
  }
  else if (user_input == 'd'){
    printf("\t\t enter new pos_0: ");
    while(1){
      pause();
      if(scanf("%f", &user_float_input) != -1)
        break;
    }
    set_pos_0(DEG2RAD(user_float_input));
    ui_menu_cb();
  }
  else if (user_input == 'f'){
    set_FB_cntrl(0);
    set_FF_cntrl(1);
    printf("\t\t Press 0 enter exit.");
    while(1){
      pause();
      if(scanf("%f", &user_float_input) != -1)
        break;
    }
    set_FF_cntrl(0);
    set_pos_0(ANGLE_OFFSET);
    ui_menu_cb();
    set_FB_cntrl(1);
  }
}


