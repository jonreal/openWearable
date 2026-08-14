#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

extern volatile sig_atomic_t input_ready;

// ---------------------------------------------------------------------------
//  ADS131M08 CLKIN -- generated in ARM space.
//
//  The part has no internal oscillator: without CLKIN the modulator never runs
//  and every SPI frame returns the same post-reset acknowledgment word instead
//  of conversions. The EHRPWM TBCLK is a Linux-owned clock the PRU cannot gate,
//  so the A8 brings it up here (and shuts it down in CpuCleanup) rather than the
//  PRU. The device tree already mounts EHRPWM2_A on P8.19 (pwm2_pins, MUX_MODE4)
//  and enables &epwmss2/&ehrpwm2, so this is pure sysfs -- no /dev/mem poking,
//  unlike the J721E path in the am64x branch, whose pwm driver can leave TBPRD=0
//  on a cold channel.
//
//  130 ns => ~7.69 MHz, the closest an AM335x 100 MHz TBCLK gets to the
//  7.8125 MHz used on am64x (exact would need a 12.8 divider).
// ---------------------------------------------------------------------------
#define CLKIN_PWM_DEV     "48304200.pwm"   // EHRPWM2
#define CLKIN_PERIOD_NS   130              // ~7.69 MHz
#define CLKIN_DUTY_NS     65               // 50 %

static char clkin_dir[64];                 // /sys/class/pwm/pwmchipN/pwm0

static int pwm_write(const char* path, const char* val) {
  FILE* fp = fopen(path, "w");
  if (fp == NULL)
    return -1;
  fputs(val, fp);
  return (fclose(fp) == 0) ? 0 : -1;
}

// Find the pwmchip index backing platform device `dev` (e.g. "48304200.pwm"), or -1.
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

// Start CLKIN on EHRPWM2_A (P8.19). Records the channel dir so PwmClkinStop can
// shut it off. Warns rather than aborting: the app is still useful without the
// ADC (e.g. checking the sync line), and a dead CLKIN is visible in the log as a
// frozen `id`/`resp`.
static void pwm_clkin_start(void) {
  int chip = pwm_find_chip(CLKIN_PWM_DEV);
  char path[96], val[32];

  if (chip < 0) {
    printf("warning: EHRPWM2 (%s) pwmchip not found -- no ADC CLKIN\n", CLKIN_PWM_DEV);
    return;
  }
  snprintf(clkin_dir, sizeof(clkin_dir), "/sys/class/pwm/pwmchip%d/pwm0", chip);

  if (access(clkin_dir, F_OK) != 0) {                  // export channel 0 if needed
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/export", chip);
    pwm_write(path, "0\n");
    snprintf(path, sizeof(path), "%s/period", clkin_dir);   // wait for udev attrs
    for (int i = 0; i < 25 && access(path, W_OK) != 0; i++) usleep(200000);
  }

  // duty first: the driver rejects a period shorter than the standing duty cycle.
  snprintf(val, sizeof(val), "%d\n", CLKIN_PERIOD_NS);
  snprintf(path, sizeof(path), "%s/duty_cycle", clkin_dir); pwm_write(path, "0\n");
  snprintf(path, sizeof(path), "%s/period",     clkin_dir); pwm_write(path, val);
  snprintf(val, sizeof(val), "%d\n", CLKIN_DUTY_NS);
  snprintf(path, sizeof(path), "%s/duty_cycle", clkin_dir); pwm_write(path, val);
  snprintf(path, sizeof(path), "%s/enable",     clkin_dir);
  if (pwm_write(path, "1\n") != 0)
    printf("warning: could not enable ADC CLKIN on %s (run as root?)\n", clkin_dir);
}

// Stop CLKIN. Called from CpuCleanup (cpuloop.c) so the clock does not keep
// running on P8.19 after the app exits.
void PwmClkinStop(void) {
  char path[96];
  if (clkin_dir[0] == '\0')
    return;
  snprintf(path, sizeof(path), "%s/enable", clkin_dir);
  pwm_write(path, "0\n");
}

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
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
    // Clear inputs
    input_char = ' ';
    input_string[0] = '\0';

    // Wait for user input.
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

          // Wait for input.
          UiPollForUserInput();
          scanf(" %s", input_string);
          strcat(log_file, input_string);
          printf("\t\tSaving data to %s\n",log_file);
          UiNewLogFile(log_file);

          // Wait for user input to start saving data
          printf("\t\tPress enter to start collection...\n");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &input_char);

          // Data collection loop
          UiStartLog();

          UiPrintMenu(pru_mem);
          break;
        }

        // ---- Stop data collection -----------------------------------------
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

// Loads this app's parameters into the shared param region. With no config
// file (file == NULL) the defaults below are used. A config file's first token
// is the sample rate in Hz; the rest of that line is a free-form comment
// (e.g. "1000  // Freq."). Defaults are written first, so a missing value in
// the file simply leaves fs_hz at its default (and never a zero -> div-by-0).
//
// Also where the ADC master clock comes up: this runs before PruInit() loads the
// firmware, so CLKIN is already stable when Pru0Init resets the ADS131M08.
int PruLoadParams(const char* file, param_mem_t* param) {

  pwm_clkin_start();          // ADS131M08 CLKIN on EHRPWM2_A (P8.19)

  // Defaults
  param->fs_hz = 1000;

  if (file != NULL) {
    FILE* fp = fopen(file, "r");
    if (fp == NULL)
      return -1;
    fscanf(fp, "%u%*[^\n]\n", &param->fs_hz);
    fclose(fp);
  }

  param->fs_ticks = HZ_TO_TICKS(param->fs_hz);

  return 0;
}
