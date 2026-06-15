/* openWearable logging benchmark — headless driver (test/logbench)
 *
 * Drop-in replacement for an app's interactive uiloop.c. It brings up the full
 * openWearable lifecycle (PRU + A8 + ring/thread logger) exactly like a normal
 * app, but instead of a menu it logs at the configured sample rate for a fixed
 * duration and then tears down. This is the "full-OW must clear" test form: the
 * whole stack runs and shuts down cleanly on every invocation.
 *
 * The benchmark question — "how fast can the A8 consume PRU state before it
 * falls behind?" — is answered by the logger's own counters:
 *   - ring-full drops: the writer thread / disk can't drain the 4 MB log ring;
 *     LogSaveFile prints "<n> records dropped" on close.
 *   - sample completeness: records captured vs fs*duration. A shortfall means
 *     the PRU state-ring (STATE_BUFF_LEN deep, drained by the fixed-rate A8
 *     SIGALRM timer) overflowed upstream of the log ring — i.e. the A8 drain
 *     cadence, not the disk, is the bottleneck.
 *
 * Knobs (environment, set by run_logbench.sh):
 *   LOGBENCH_SECS  run length in seconds            (default 10)
 *   LOGBENCH_SINK  directory to write the log into  (default "datalog")
 *
 * Sample rate (fs) comes from the -c config file via PruLoadParams, like any app.
 */

#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Headless: no interactive menu.
void UiPrintMenu(const pru_mem_t* pru_mem) {
  (void) pru_mem;
}

static double EnvDouble(const char* name, double dflt) {
  const char* s = getenv(name);
  if (!s || !*s)
    return dflt;
  double v = atof(s);
  return (v > 0.0) ? v : dflt;
}

int UiLoop(const pru_mem_t* pru_mem) {
  double secs = EnvDouble("LOGBENCH_SECS", 10.0);
  const char* sink = getenv("LOGBENCH_SINK");
  if (!sink || !*sink)
    sink = "datalog";

  uint32_t fs = pru_mem->p->fs_hz;
  char path[512];
  snprintf(path, sizeof(path), "%s/logbench_%uhz.owl", sink, fs);

  printf("RUN fs_hz=%u secs=%.1f file=%s\n", fs, secs, path);
  fflush(stdout);

  UiNewLogFile(path);                       // opens file + starts writer thread
  UiStartLog();                             // timer ISR now packs records

  // Logging runs entirely in the SIGALRM timer callback; we just wait out the
  // run. usleep may return early when the timer fires (EINTR) — harmless, the
  // wall-clock check below governs the duration.
  struct timespec t0, now;
  clock_gettime(CLOCK_MONOTONIC, &t0);
  do {
    usleep(100000);                         // 100 ms
    clock_gettime(CLOCK_MONOTONIC, &now);
  } while (((now.tv_sec - t0.tv_sec) + (now.tv_nsec - t0.tv_nsec) / 1e9) < secs);

  UiStopAndSaveLog();                       // drains ring, prints drops, closes

  printf("DONE fs_hz=%u file=%s\n", fs, path);
  fflush(stdout);
  return 1;                                 // tells main.c to disable PRU + clean up
}

// Per-app parameter hook (same contract as apps/template's uiloop.c): main.c
// calls this during init to populate the shared param region. The sweep driver
// sets fs by writing it as the first token of the -c config file.
int PruLoadParams(const char* file, param_mem_t* param) {

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
