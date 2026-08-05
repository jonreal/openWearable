#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

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

// The R5F drives the McSPI7 registers directly but can't enable a J721E functional
// clock, so Linux must hold McSPI7's clock on (runtime-PM). Do it here, before the
// R5F starts, so the binary stays self-contained (no setup script).
static void pin_clock_on(const char* dev) {
  char path[128];
  snprintf(path, sizeof(path), "/sys/bus/platform/devices/%s/power/control", dev);
  FILE* f = fopen(path, "w");
  if (f) { fputs("on\n", f); fclose(f); }
  else   { printf("warning: could not pin clock for %s (run as root?)\n", dev); }
}

static int pwm_write(const char* path, const char* val) {
  FILE* f = fopen(path, "w");
  if (!f) return -1;
  fputs(val, f);
  fclose(f);
  return 0;
}

// Find the pwmchip index backing platform device `dev` (e.g. "3020000.pwm"), or -1.
static int pwm_find_chip(const char* dev) {
  for (int n = 0; n < 16; n++) {
    char link[64], target[256];
    snprintf(link, sizeof(link), "/sys/class/pwm/pwmchip%d/device", n);
    ssize_t k = readlink(link, target, sizeof(target) - 1);
    if (k < 0) continue;
    target[k] = '\0';
    const char* base = strrchr(target, '/');
    base = base ? base + 1 : target;
    if (strcmp(base, dev) == 0) return n;
  }
  return -1;
}

// Generate the ADS131M08 CLKIN on EHRPWM2_A (P9.14) with the Linux pwm framework.
// The EHRPWM TBCLK is gated by pwm-tiehrpwm and can only be ungated by a userspace
// consumer -- the R5F cannot do it. The pinmux (mode 6 = EHRPWM2_A) is set by the
// DTB. EHRPWM2 = 3020000.pwm; channel 0 = EHRPWM2_A. 128 ns -> 7.8125 MHz at 50%
// duty (within the ADS131M08 CLKIN spec for every PGA gain; see README.md).
static void pwm_clkin_start(void) {
  int chip = pwm_find_chip("3020000.pwm");
  if (chip < 0) { printf("warning: EHRPWM2 (3020000.pwm) pwmchip not found\n"); return; }
  char dir[48], path[80];
  snprintf(dir, sizeof(dir), "/sys/class/pwm/pwmchip%d", chip);

  snprintf(path, sizeof(path), "%s/pwm0", dir);
  if (access(path, F_OK) != 0) {            // export channel 0 if not already
    snprintf(path, sizeof(path), "%s/export", dir);
    pwm_write(path, "0\n");
  }
  snprintf(path, sizeof(path), "%s/pwm0/period",     dir); pwm_write(path, "128\n");
  snprintf(path, sizeof(path), "%s/pwm0/duty_cycle", dir); pwm_write(path, "64\n");
  snprintf(path, sizeof(path), "%s/pwm0/enable",     dir); pwm_write(path, "1\n");
}

int PruLoadParams(const char* file, param_mem_t* param) {

  pin_clock_on("2170000.spi");   // McSPI7 SPI clock on for the R5F
  pwm_clkin_start();             // ADS131M08 CLKIN on EHRPWM2_A (P9.14)

  // Defaults
  param->fs_hz = 1000;
  param->fs_ticks = HZ_TO_TICKS(param->fs_hz);
  param->r5f_decimate = 10;   // ADC read at fs/decimate = 100 Hz

  return 0;
}
