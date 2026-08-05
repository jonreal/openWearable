#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>

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

// Program EHRPWM2 registers directly for a guaranteed 7.8125 MHz / 50% CLKIN and
// force P9.14 -> EHRPWM2_A (mode 6). This is the fix for the cold-boot case: the
// pwm driver's period apply can leave TBPRD=0 / counter frozen on a fresh channel,
// so once TBCLK is ungated (by the sysfs enable below) we set the waveform by hand.
// Mirrors apps/test-ads131m08/enable-pwm.sh. TBCLK = 125 MHz.
static void epwm_program_clkin(void) {
  int fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 0) { printf("warning: open /dev/mem for CLKIN failed (run as root?)\n"); return; }
  volatile uint8_t*  mmr  = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x0011C000); // CTRL_MMR pads
  volatile uint8_t*  epwm = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x03020000); // EHRPWM2
  if (mmr != MAP_FAILED && epwm != MAP_FAILED) {
    volatile uint16_t* e = (volatile uint16_t*)epwm;
    *(volatile uint32_t*)(mmr + 0x178) = 0x00010006u;  // P9.14 pad -> EHRPWM2_A (mode 6)
    e[0x00 / 2] = 0x0003;   // TBCTL  freeze while configuring
    e[0x08 / 2] = 0x0000;   // TBCNT  = 0
    e[0x0A / 2] = 0x000F;   // TBPRD  = 15 -> 125MHz/16 = 7.8125 MHz
    e[0x0E / 2] = 0x0000;   // CMPCTL (CMPA shadow-loads at CTR=0)
    e[0x12 / 2] = 0x0008;   // CMPA   = 8 (~50%)
    e[0x16 / 2] = 0x0012;   // AQCTLA set@zero, clear@CMPA
    e[0x00 / 2] = 0x0000;   // TBCTL  up-count -> run
  } else {
    printf("warning: mmap for CLKIN failed\n");
  }
  if (mmr  != MAP_FAILED) munmap((void*)mmr,  0x1000);
  if (epwm != MAP_FAILED) munmap((void*)epwm, 0x1000);
  close(fd);
}

// Generate the ADS131M08 CLKIN on EHRPWM2_A (P9.14), self-contained. The J721E
// EHRPWM TBCLK is a Linux-owned clock the R5F cannot gate; enabling the pwm channel
// ungates it. On a cold (fresh-boot) channel the driver's period apply can leave
// TBPRD=0, so we (1) enable the channel to ungate TBCLK, then (2) program the EPWM
// registers directly. CLKIN = 7.8125 MHz -- in ADS131M08 spec for every PGA gain.
// (Manual equivalent: apps/test-ads131m08/enable-pwm.sh. Pinmux also set by DTB.)
static void pwm_clkin_start(void) {
  int chip = pwm_find_chip("3020000.pwm");
  if (chip < 0) { printf("warning: EHRPWM2 (3020000.pwm) pwmchip not found\n"); return; }
  char dir[48], path[80];
  snprintf(dir, sizeof(dir), "/sys/class/pwm/pwmchip%d", chip);

  snprintf(path, sizeof(path), "%s/pwm0", dir);
  if (access(path, F_OK) != 0) {                 // export channel 0 if needed
    snprintf(path, sizeof(path), "%s/export", dir);
    pwm_write(path, "0\n");
    snprintf(path, sizeof(path), "%s/pwm0/period", dir);       // wait for udev attrs
    for (int i = 0; i < 25 && access(path, W_OK) != 0; i++) usleep(200000);
  }
  // enable -> ungate TBCLK (the sysfs state needs a nonzero period first)
  snprintf(path, sizeof(path), "%s/pwm0/period",     dir); pwm_write(path, "128\n");
  snprintf(path, sizeof(path), "%s/pwm0/duty_cycle", dir); pwm_write(path, "64\n");
  snprintf(path, sizeof(path), "%s/pwm0/enable",     dir); pwm_write(path, "1\n");

  epwm_program_clkin();   // guarantee the waveform (fixes the cold-boot TBPRD=0)
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
