#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

#include "gpio.h"
#include "control.h"
#include "pru_wrappers.h"
#include "tui.h"

#define DEBUG_PIN "P8_15"

tui_t *ptui;

uint32_t gpio_debug;

/* ----------------------------------------------------------------------------
 * Function init_tui()
 * ------------------------------------------------------------------------- */
int init_tui(void)
{
  struct sigaction action_ui;

  ptui = malloc(sizeof(tui_t));
  ptui->io_ready = 0;

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
//  add_func_to_cleanup(&tui_cleanup);
  printf("TUI initialized.\n");

  /* Debug pin */
  /* set enable pins */
  if(get_gpio_number(DEBUG_PIN, &gpio_debug) != 0){
    printf("Error getting gpio number.\n");
    return -1;
  }
  gpio_export(gpio_debug);
  gpio_set_direction(gpio_debug, OUTPUT);

  return 0;
}

/* ----------------------------------------------------------------------------
 * Function io_cb()
 * ------------------------------------------------------------------------- */
void io_cb(int sig)
{
  ptui->io_ready = 1;
}

/* ----------------------------------------------------------------------------
 * Function ui_menu_cb()
 * ------------------------------------------------------------------------- */
void tui_menu(void)
{
  printf("\n\n--------------------------\n");
  printf("Kp = %d, Kd = %d, pos0 = %d\n", getKp(), getKd(), getAnklePos0());
  printf("Menu: a - enter new Kp\n");
  printf("      s - enter new Kd\n");
  printf("      d - enter new pos_0 (deg X 100)\n");
  printf("      f - collect trial\n");
  printf("      g - tare encoder\n");
  printf("      h - enable/disable feedforward control\n");
  printf("      e - exit\n");
  printf("--------------------------\n");
}

/* ----------------------------------------------------------------------------
 * Function tui()
 * ------------------------------------------------------------------------- */
int start_tui(void)
{
  char lastBufferRead = 0;
  char input_char;
  float input_float;
  char input_string[256];
  char logFile[256] = "datalog/";

  tui_menu();
  fflush(stdout);
  while(1){

    /* Get top level input */
    if(ptui->io_ready){
      scanf(" %c", &input_char);

      /* Exit */
      if(input_char == 'e'){
        return 1;
      }

      /* Update Kp */
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
        setKp(input_float);
        tui_menu();
        fflush(stdout);
      }

      /* Update Kd */
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
        setKd(input_float);
        tui_menu();
        fflush(stdout);
      }

      /* Update pos_0 */
      else if(input_char == 'd'){
        printf("\t\tEnter new pos_0 (deg X 100): ");
        fflush(stdout);
        ptui->io_ready = 0;
        while(1){
          if(ptui->io_ready){
            scanf(" %f", &input_float);
            break;
          }
        }
        setAnklePos0(input_float);
        tui_menu();
        fflush(stdout);
      }

      /* Log trial */
      else if(input_char == 'f'){

        /* Trial Name Input */
        printf("\t\tEnter trial name: ");
        fflush(stdout);
        ptui->io_ready = 0;
        while(1){
          if(ptui->io_ready){
            scanf(" %s", input_string);
            break;
          }
        }

        /* Echo Trial Name */
        strcat(logFile,input_string);
        printf("\t\tSaving data to %s\n",logFile);
        logFileInit(logFile);

        /* Wait for enter to start saving data */
        printf("\t\tPress enter to start collection...\n");
        fflush(stdout);
        ptui->io_ready = 0;
        while(1){
          if(ptui->io_ready){
            scanf(" %c", &input_char);
            break;
          }
        }

        /* Wait for enter to stop collection */
        printf("\t\tPress enter to stop collection...\n");
        fflush(stdout);
        ptui->io_ready = 0;

        clearBufferFlags();
        lastBufferRead = 0;

        /* Data collection loop */
        while(1){

          /* Log Data Here */
          if(isBuffer0Full()){
            resetBuffer0FullFlag();
            gpio_set_value(gpio_debug, HIGH);
            writeState(0);
            lastBufferRead = 0;
            gpio_set_value(gpio_debug, LOW);
          }
          if(isBuffer1Full()){
            resetBuffer1FullFlag();
            gpio_set_value(gpio_debug, HIGH);
            writeState(1);
            lastBufferRead = 1;
            gpio_set_value(gpio_debug, LOW);
          }

          /* Check for input */
          if(ptui->io_ready){
            scanf(" %c", &input_char);
            break;
          }
        }

        /* Get last buffers */
        if(lastBufferRead == 0){
          writeState(1);
        }
        else{
          writeState(0);
        }
        closeLogFile();
        tui_menu();
        fflush(stdout);
        ptui->io_ready = 0;
      }

      /* Encoder Tare */
      else if(input_char == 'g'){
        setTareEncoderBit();
        printf("\t\tEncoder angle reset.\n");
        fflush(stdout);
        ptui->io_ready = 0;
        tui_menu();
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
