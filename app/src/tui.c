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
//#include "control.h"
#include "common.h"
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
  char side;

  if (getProsSide() == 1)
    side = 'L';
  else
    side = 'R';

  printf("\n\n---------------------------------------------------------------------\n"
          "hs_delay = %i, u_bias = %3.2f, ProsSide = %c, FF enabled = %d, FFgain = %3.2f\n"
          "Menu: a - Enter new hs_delay\n"
          "      s - Enter new u_bias\n"
          "      d - Change prosthetic limb side\n"
          "      f - Collect trial\n"
          "      g - Save parameters\n"
          "      h - Load parameters\n"
          "      j - Load Feedforward lookup table\n"
          "      k - Toggle feedforward control\n"
          "      l - Enter new FFgain\n"
          "      p - Test Feedforward\n"
          "      q - Step Response\n"
          "      e - exit\n"
          "-----------------------------------------------------------------------\n",
          geths_delay(), getu_bias(), side, getFFenable(), getFFgain());
  fflush(stdout);
}

/* ----------------------------------------------------------------------------
 * Function tui()
 * ------------------------------------------------------------------------- */
int start_tui(void)
{
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

        // ---- Set hs_delay --------------------------------------------------
        case 'a' :
          printf("\t\tEnter new hs_delay: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %f", &inFloat);
          seths_delay((uint32_t)inFloat);
          tui_menu();
          ptui->io_ready = 0;
          break;

        // ---- Set u_bias ----------------------------------------------------
        case 's' :
          printf("\t\tEnter new u_bias: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %f", &inFloat);
          setu_bias(inFloat);
          tui_menu();
          ptui->io_ready = 0;
          break;

        // ---- Set ProSide------------------------------------------------------
        case 'd' :
          printf("\t\tChange ProsSide [y/n]? ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %c", &inChar);
          if (inChar == 'y')
            setProsSide( !getProsSide());
          tui_menu();
          ptui->io_ready = 0;
          break;

        // ---- Collect trial -------------------------------------------------
        case 'f' :
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

        // ---- Save Parameters -----------------------------------------------
        case 'g' :
          printf("\t\tEnter parameter file name: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %s", inString);

          /* Echo Trial Name */
          strcat(configFile, inString);
          printf("\t\tSaving parameters to %s\n",configFile);
          fflush(stdout);
          saveParameters(configFile);
          configFile[0] = '\0';
          strcat(configFile, "config/");
          tui_menu();
          ptui->io_ready = 0;
          break;

        // ---- Load Parameters -----------------------------------------------
        case 'h' :
          printf("\t\tEnter parameter file name: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %s", inString);

          /* Echo Trial Name */
          strcat(configFile, inString);
          printf("\t\tLoading parameters from %s\n",configFile);
          fflush(stdout);
          loadParameters(configFile);
          configFile[0] = '\0';
          strcat(configFile, "config/");
          tui_menu();
          ptui->io_ready = 0;
          break;

        // ---- Load Lookup -----------------------------------------------
        case 'j' :
          printf("\t\tEnter lookup table file name: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %s", inString);

          /* Echo Trial Name */
          strcat(configFile, inString);
          printf("\t\tLoading lookup table from %s\n",configFile);
          fflush(stdout);
          loadLookUpTable(configFile);
          configFile[0] = '\0';
          strcat(configFile, "config/");
          tui_menu();
          ptui->io_ready = 0;
          break;


        // ---- Toggle feedforward -----------------------------------------------
        case 'k' :
          setFFenable( getFFenable() ^ 1);
          printf("\t\tFeedforward toggled.\n");
          fflush(stdout);
          ptui->io_ready = 0;
          tui_menu();
          break;

        // ---- FFgain -----------------------------------------------
        case 'l' :

          printf("\t\tEnter new ffgain: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %f", &inFloat);
          setFFgain(inFloat);
          tui_menu();
          ptui->io_ready = 0;
          break;

        // ---- Test FF -----------------------------------------------
        case 'p' :
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

          // Init
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


          startFFtest();

          // Wait for enter to stop collection
          printf("\t\tPress enter to stop collection...\n");
          fflush(stdout);
          ptui->io_ready = 0;

          // Data collection
          while(1){

            logData();

            // Check for input
            if(ptui->io_ready)
              break;
          }
          scanf(" %c", &inChar);
          stopFFtest();

          closeLogFile();
          logFile[0] = '\0';
          strcat(logFile, "datalog/");
          tui_menu();
          fflush(stdout);
          ptui->io_ready = 0;
          break;

        // ---- Step Response -----------------------------------------------
        case 'q' :
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

          // Init
          logFileInit(logFile);
          circBuffInit();

          printf("\t\tEnter demand current: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %f", &inFloat);
          setStepCurrent(inFloat);

          // Wait for enter to start saving data
          printf("\t\tPress enter to start collection...\n");
          fflush(stdout);
          ptui->io_ready = 0;
          while(1)
            if(ptui->io_ready)
              break;
          scanf(" %c", &inChar);

          startStepResponse();

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
          printf("\t\tReset step resp. vars.\n");
          resetStepRespVars();

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
