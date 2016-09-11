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
  printf("\n\n---------------------------------------------------------------------\n"
          "Haptic Mode = %d\n"
          "Menu: a - Change Haptic Mode\n"
          "      z - Manual Mode\n"
          "      s - Collect trial\n"
          "      e - exit\n"
          "-----------------------------------------------------------------------\n",
          getHapticMode());
  fflush(stdout);
}

/* ----------------------------------------------------------------------------
 * Function tui()
 * ------------------------------------------------------------------------- */
int start_tui(void)
{
  char lastBufferRead = 0;
  char inChar;
  float inFloat;
  char inString[256];

  char logFile[256] = "datalog/";

  char configFile[256] = "config/";

  tui_menu();
  fflush(stdout);
  while(1){

    // Clear inputs
    inChar = ' ';
    inFloat = 0.0;
    inString[0] = '\0';

    // Wait for user input.
    if(ptui->io_ready){
      scanf(" %c", &inChar);

      switch(inChar){

        // ---- Exit ----------------------------------------------------------
        case 'e' :
          return 1;

        // ---- Set Haptic Mode -----------------------------------------------
        case 'a' :
          printf("\t\tEnter new mode (0 | 1 | 2 | 3): ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %d", &inChar);
          setHapticMode(inChar);
          tui_menu();
          ptui->io_ready = 0;
          break;

//        // ---- Manual Mode -----------------------------------------------
//        case 'a' :
//          while(1) {
//            printf("\t\tManual mode \n
//                    \t\t\t motor 1 : %s\n
//                    \t\t\t motor 2 : %s\n
//                    \t\t\t motor 3 : %s\n
//                    \t\t\t motor 4 : %s\n
//                    \t\t\t motor 5 : %s\n
//                    \t\t\t motor 5 : %s\n",
//                    p->hapticFlag[0], p->hapticFlag[1], 
//          }
//
//
//
//
//
//          fflush(stdout);
//          ptui->io_ready = 0;
//
//          // Wait for user input.
//          while(1)
//            if(ptui->io_ready)
//              break;
//
//          scanf(" %d", &inChar);
//          setHapticMode(inChar);
//          tui_menu();
//          ptui->io_ready = 0;
//          break;
//
//
        // ---- Collect trial -------------------------------------------------
        case 's' :
          printf("\t\tEnter trial name: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %s", inString);
          strcat(logFile, inString);
          printf("\t\tSaving data to %s\n",logFile);

          // Init file and circbuff
          logFileInit(logFile);
          circBuffInit();

          // Wait for enter to start saving data
          printf("\t\tPress enter to start collection...\n");
          fflush(stdout);
          ptui->io_ready = 0;
          while(1)
            if(ptui->io_ready)
              break;
          scanf(" %c", &inChar);

          // Wait for enter to stop collection
          printf("\t\tPress enter to stop collection...\n");
          fflush(stdout);
          ptui->io_ready = 0;

          // Data collection loop
          while(1){

            logData();

            // Check for input
            if(ptui->io_ready)
              break;
          }
          scanf(" %c", &inChar);

          closeLogFile();
          logFile[0] = '\0';
          strcat(logFile, "datalog/");
          tui_menu();
          fflush(stdout);
          ptui->io_ready = 0;
          break;
      }
    }
  }
}

void logData(void)
{
 circBuffUpdate();
 gpio_set_value(gpio_debug, HIGH);
 writeState();
 gpio_set_value(gpio_debug, LOW);
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
