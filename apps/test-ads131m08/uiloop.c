#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "test-ads131m08 -- R5F reads the 8 ADC channels over McSPI7\n\n"
  "Menu: s - start log\n"
  "      d - stop log\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n");
  fflush(stdout);
}

int UiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  char input_string[256] = {0};
  char log_file[256] = "datalog/";

  UiPrintMenu(pru_mem);
  while (1) {
    input_char = ' ';
    input_string[0] = '\0';

    if (input_ready) {
      scanf(" %c", &input_char);

      switch(input_char){

        // ---- Exit ----------------------------------------------------------
        case 'e' : {
          UiStopAndSaveLog();
          printf("done.\n");
          return 1;
        }

        // ---- Start data collection -----------------------------------------
        case 's' : {
          printf("\t\tEnter trial name: ");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n", log_file);
          UiNewLogFile(log_file);

          printf("\t\tPress enter to start collection...\n");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &input_char);

          UiStartLog();
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- Stop data collection ------------------------------------------
        case 'd' : {
          if (!UiLogging())
            printf("\t\t Not currently logging data!\n");
          else
            UiStopAndSaveLog();

          log_file[0] = '\0';
          strcat(log_file, "datalog/");
          UiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}

// Keep a peripheral's functional clock from autosuspending: the R5F drives
// EHRPWM2 (CLKIN) and McSPI7 (SPI) registers directly but can't enable a J721E
// clock, so Linux must hold them on. Doing it here -- before the R5F starts --
// makes the binary self-contained (no adc-setup.sh needed).
static void pin_clock_on(const char* dev) {
  char path[128];
  snprintf(path, sizeof(path), "/sys/bus/platform/devices/%s/power/control", dev);
  FILE* f = fopen(path, "w");
  if (f) { fputs("on\n", f); fclose(f); }
  else   { printf("warning: could not pin clock for %s (run as root?)\n", dev); }
}

int PruLoadParams(const char* file, param_mem_t* param) {

  // Hold the ADS131M08's CLKIN (EHRPWM2) and SPI (McSPI7) clocks on for the R5F.
  pin_clock_on("3020000.pwm");   // EHRPWM2 -> CLKIN (P9.14)
  pin_clock_on("2170000.spi");   // McSPI7  -> SPI

  // Defaults
  param->fs_hz = 1000;
  param->fs_ticks = HZ_TO_TICKS(param->fs_hz);
  param->r5f_decimate = 10;   // ADC read at fs/decimate = 100 Hz

  return 0;
}
