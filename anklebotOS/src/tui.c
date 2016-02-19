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
  printf("\n\n------------------------------------------------------------\n"
          "Kp = %3.2f, Kd = %3.2f, pos0 = %3.2f, FF enabled = %d\n"
          "Menu: a - Enter new Kp\n"
          "      s - Enter new Kd\n"
          "      d - Enter new pos_0 (deg)\n"
          "      f - Collect trial\n"
          "      g - Save parameters\n"
          "      h - Load parameters\n"
          "      j - Load Feedforward lookup table\n"
          "      k - Toggle feedforward control\n"
          "      l - Reset gait phase detection\n"
          "      o - Tare encoder angle\n"
          "      p - Test Feedforward\n"
          "      q - Step Response\n"
          "      e - exit\n"
          "--------------------------------------------------------------\n",
          getKp(), getKd(), getAnklePos0(), FFenabled());
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

        // ---- Exit -------------------------------------------------------------
        case 'e' :
          return 1;

        // ---- Set Kp -----------------------------------------------------------
        case 'a' :
          printf("\t\tEnter new Kp: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %f", &inFloat);
          setKp(inFloat);
          tui_menu();
          ptui->io_ready = 0;
          break;

        // ---- Set Kd -----------------------------------------------------------
        case 's' :
          printf("\t\tEnter new Kd: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %f", &inFloat);
          setKd(inFloat);
          tui_menu();
          ptui->io_ready = 0;
          break;

        // ---- Set pos0 ---------------------------------------------------------
        case 'd' :
          printf("\t\tEnter new pos0: ");
          fflush(stdout);
          ptui->io_ready = 0;

          // Wait for user input.
          while(1)
            if(ptui->io_ready)
              break;

          scanf(" %f", &inFloat);
          setAnklePos0(inFloat);
          tui_menu();
          ptui->io_ready = 0;
          break;

        // ---- Collect trial ----------------------------------------------------
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
          logFileInit(logFile);

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
          clearBufferFlags();
          lastBufferRead = 0;
          while(1){
            if(buffer0Full()){
              clearBuffer0FullFlag();
              gpio_set_value(gpio_debug, HIGH);
              writeState(0);
              lastBufferRead = 0;
              gpio_set_value(gpio_debug, LOW);
            }
            else if(buffer1Full()){
              clearBuffer1FullFlag();
              gpio_set_value(gpio_debug, HIGH);
              writeState(1);
              lastBufferRead = 1;
              gpio_set_value(gpio_debug, LOW);
            }

            /* Check for input */
            if(ptui->io_ready)
              break;
          }
          scanf(" %c", &inChar);

          // Get last buffer
          if(lastBufferRead == 0)
            writeState(1);
          else
            writeState(0);

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

        // ---- Load Parameters -----------------------------------------------
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
          loadParameters(configFile);
          configFile[0] = '\0';
          strcat(configFile, "config/");
          tui_menu();
          ptui->io_ready = 0;
          break;


        // ---- Toggle feedforward -----------------------------------------------
        case 'k' :
          enableFF( FFenabled() ^ 1);
          printf("\t\tFeedforward toggled.\n");
          fflush(stdout);
          ptui->io_ready = 0;
          tui_menu();
          break;

        // ---- Reset gait phase -----------------------------------------------
        case 'l' :

          //TODO add method here.
          printf("\t\tGait phase reset.\n");
          fflush(stdout);
          ptui->io_ready = 0;
          tui_menu();
          break;

        // ---- Tare encoder -----------------------------------------------
        case 'o' :
          setTareEncoderBit();
          printf("\t\tEncoder angle zeroed.\n");
          fflush(stdout);
          ptui->io_ready = 0;
          tui_menu();
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
          logFileInit(logFile);

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

          // Data collection loop
          clearBufferFlags();
          lastBufferRead = 0;
          while(1){
            if(buffer0Full()){
              clearBuffer0FullFlag();
              gpio_set_value(gpio_debug, HIGH);
              writeState(0);
              lastBufferRead = 0;
              gpio_set_value(gpio_debug, LOW);
            }
            else if(buffer1Full()){
              clearBuffer1FullFlag();
              gpio_set_value(gpio_debug, HIGH);
              writeState(1);
              lastBufferRead = 1;
              gpio_set_value(gpio_debug, LOW);
            }

            /* Check for input */
            if(ptui->io_ready)
              break;
          }
          scanf(" %c", &inChar);
          stopFFtest();

          // Get last buffer
          if(lastBufferRead == 0)
            writeState(1);
          else
            writeState(0);

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
          logFileInit(logFile);

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
          clearBufferFlags();
          lastBufferRead = 0;
          while(1){
            if(buffer0Full()){
              clearBuffer0FullFlag();
              gpio_set_value(gpio_debug, HIGH);
              writeState(0);
              lastBufferRead = 0;
              gpio_set_value(gpio_debug, LOW);
            }
            else if(buffer1Full()){
              clearBuffer1FullFlag();
              gpio_set_value(gpio_debug, HIGH);
              writeState(1);
              lastBufferRead = 1;
              gpio_set_value(gpio_debug, LOW);
            }

            /* Check for input */
            if(ptui->io_ready)
              break;
          }
          scanf(" %c", &inChar);
          printf("\t\tReset step resp. vars.\n");
          resetStepRespVars();

          // Get last buffer
          if(lastBufferRead == 0)
            writeState(1);
          else
            writeState(0);

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


//      if(input_char == 'e'){
//        return 1;
//      }
//
//      /* Update Kp */
//      else if(input_char == 'a'){
//        printf("\t\tEnter new Kp: ");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        while(1){
//          if(ptui->io_ready){
//            scanf(" %f", &input_float);
//            break;
//          }
//        }
//        setKp(input_float);
//        tui_menu();
//        fflush(stdout);
//      }
//
//      /* Update Kd */
//      else if(input_char == 's'){
//        printf("\t\tEnter new Kd: ");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        while(1){
//          if(ptui->io_ready){
//            scanf(" %f", &input_float);
//            break;
//          }
//        }
//        setKd(input_float);
//        tui_menu();
//        fflush(stdout);
//      }
//
//      /* Update pos_0 */
//      else if(input_char == 'd'){
//        printf("\t\tEnter new pos_0 (deg X 100): ");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        while(1){
//          if(ptui->io_ready){
//            scanf(" %f", &input_float);
//            break;
//          }
//        }
//        setAnklePos0(input_float);
//        tui_menu();
//        fflush(stdout);
//      }
//
//      /* Log trial */
//      else if(input_char == 'f'){
//
//        /* Trial Name Input */
//        printf("\t\tEnter trial name: ");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        while(1){
//          if(ptui->io_ready){
//            scanf(" %s", input_string);
//            break;
//          }
//        }
//
//        /* Echo Trial Name */
//        strcat(logFile,input_string);
//        printf("\t\tSaving data to %s\n",logFile);
//        logFileInit(logFile);
//
//        /* Wait for enter to start saving data */
//        printf("\t\tPress enter to start collection...\n");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        while(1){
//          if(ptui->io_ready){
//            scanf(" %c", &input_char);
//            break;
//          }
//        }
//
//        /* Wait for enter to stop collection */
//        printf("\t\tPress enter to stop collection...\n");
//        fflush(stdout);
//        ptui->io_ready = 0;
//
//        clearBufferFlags();
//        lastBufferRead = 0;
//
//        /* Data collection loop */
//        while(1){
//
//          /* Log Data Here */
//          if(buffer0Full()){
//            clearBuffer0FullFlag();
//            gpio_set_value(gpio_debug, HIGH);
//            writeState(0);
//            lastBufferRead = 0;
//            gpio_set_value(gpio_debug, LOW);
//          }
//          if(buffer1Full()){
//            clearBuffer1FullFlag();
//            gpio_set_value(gpio_debug, HIGH);
//            writeState(1);
//            lastBufferRead = 1;
//            gpio_set_value(gpio_debug, LOW);
//          }
//
//          /* Check for input */
//          if(ptui->io_ready){
//            scanf(" %c", &input_char);
//            break;
//          }
//        }
//
//        /* Get last buffers */
//        if(lastBufferRead == 0){
//          writeState(1);
//        }
//        else{
//          writeState(0);
//        }
//        closeLogFile();
//        logFile[0] = '\0';
//        strcat(logFile, "datalog/");
//        tui_menu();
//        fflush(stdout);
//        ptui->io_ready = 0;
//      }
//
//      /* Save Parameters */
//      else if(input_char == 'g'){
//
//        /* Trial Name Input */
//        printf("\t\tEnter parameter file name: ");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        while(1){
//          if(ptui->io_ready){
//            scanf(" %s", input_string);
//            break;
//          }
//        }
//
//        /* Echo Trial Name */
//        strcat(configFile,input_string);
//        printf("\t\tSaving parameters to %s\n",configFile);
//        saveParameters(configFile);
//        configFile[0] = '\0';
//        strcat(configFile, "config/");
//        tui_menu();
//        fflush(stdout);
//        ptui->io_ready = 0;
//      }
//
//      /* Load Parameters */
//      else if(input_char == 'h'){
//
//        /* Trial Name Input */
//        printf("\t\tEnter parameter file name: ");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        while(1){
//          if(ptui->io_ready){
//            scanf(" %s", input_string);
//            break;
//          }
//        }
//
//        /* Echo Trial Name */
//        strcat(configFile,input_string);
//        printf("\t\tLoading parameters from %s\n",configFile);
//        loadParameters(configFile);
//        configFile[0] = '\0';
//        strcat(configFile, "config/");
//        tui_menu();
//        fflush(stdout);
//        ptui->io_ready = 0;
//      }
//
//      /* Toggle Feedforward */
//      else if(input_char == 'j'){
//
//        enableFF( FFenabled() ^ 1);
//        printf("\t\tFeedforward toggeled.\n");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        tui_menu();
//      }
//
//      /* Reset GaitPhase */
//      else if(input_char == 'k'){
//        resetGaitPhase();
//        printf("\t\tGait Phase Detection reset.\n");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        tui_menu();
//      }
//
//      /* Encoder Tare */
//      else if(input_char == 'l'){
//        setTareEncoderBit();
//        printf("\t\tEncoder angle reset.\n");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        tui_menu();
//      }
//
//      /* Test FF */
//      else if(input_char == 'p'){
//        printf("\t\tTesting Feedforward.\n");
//        printf("\t\t\tPress enter to stop.\n");
//        testFF();
//        fflush(stdout);
//        ptui->io_ready = 0;
//
//        /* Testing loop */
//        while(1){
//          /* Check for input */
//          if(ptui->io_ready){
//            scanf(" %c", &input_char);
//            stopTestFF();
//            break;
//          }
//        }
//        tui_menu();
//      }
//
//      /* Load FF lookup table */
//      else if(input_char == 'o'){
//
//        /* Trial Name Input */
//        printf("\t\tEnter FF lookup table file name: ");
//        fflush(stdout);
//        ptui->io_ready = 0;
//        while(1){
//          if(ptui->io_ready){
//            scanf(" %s", input_string);
//            break;
//          }
//        }
//
//        /* Echo Trial Name */
//        strcat(configFile,input_string);
//        printf("\t\tLoading lookup table from %s\n",configFile);
//
//        /* Toggle FF if on */
//        if(FFenabled())
//          enableFF(0);
//
//        loadLookUpTable(configFile);
//        configFile[0] = '\0';
//        strcat(configFile, "config/");
//        tui_menu();
//        fflush(stdout);
//        ptui->io_ready = 0;
//      }
//      ptui->io_ready = 0;
//    }
//  }

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
