#include "ui.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern volatile sig_atomic_t input_ready;

// ---- experiment session state (file-scope so UiPrintMenu can show it) --------
static char    participant[128] = "default";
static char    condition        = 'A';   // 'A' active (assist), 'P' passive (Pd = 0)
static int     trial_count      = 0;      // resets when participant or condition changes
static fix16_t active_ptarget;            // assist level for Active trials (seeded in UiLoop)

// Passive means the PAM never inflates -> command Pd = 0. Active applies the
// stored assist level. Keeping active_ptarget separate from param->Ptarget lets
// 'a' set the assist level even while Passive; it takes effect on switch to Active.
static void ApplyCondition(const pru_mem_t* pru_mem) {
  pru_mem->p->Ptarget = (condition == 'P') ? 0 : active_ptarget;
}

void UiPrintMenu(const pru_mem_t* pru_mem) {
  printf(
  "\n\n---------------------------------------------------------------------\n"
  "Participant: %s\tCondition: %c\tNext trial: %d\n"
  "Assist Ptarget = %3.2f%s\tTdelay = %i\tTtrial = %i\n\n"
  "Menu: t - collect trial\n"
  "      p - set participant ID\n"
  "      c - set condition [A/P]\n"
  "      a - change Ptarget (assist level)\n"
  "      s - change Tdelay\n"
  "      d - change Ttrial\n"
  "      e - exit\n"
  "-----------------------------------------------------------------------\n",
  participant, condition, trial_count + 1,
  fix16_to_float(active_ptarget),
  (condition == 'P') ? " (passive: Pd=0)" : "",
  pru_mem->p->Tdelay,
  pru_mem->p->Ttrial);
  fflush(stdout);
}

int UiLoop(const pru_mem_t* pru_mem) {
  char input_char = 0;
  char log_file[256] = {0};
  float input_float = 0;

  active_ptarget = pru_mem->p->Ptarget;   // seed from PruLoadParams default
  ApplyCondition(pru_mem);                 // start consistent with the condition

  UiPrintMenu(pru_mem);
  while (1) {
    // Clear inputs
    input_char = ' ';

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

        // ---- Collect trial (auto-named: datalog/<id>_<cond>_<n>.bin) -------
        case 't' : {
          int n = trial_count + 1;
          snprintf(log_file, sizeof(log_file), "datalog/%s_%c_%d.bin",
                   participant, condition, n);
          printf("\t\tSaving data to %s\n", log_file);
          UiNewLogFile(log_file);

          // Wait for user input to start saving data
          printf("\t\tPress enter to start trial...\n");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &input_char);

          // Data collection loop
          printf("\t\tTrial collection ongoing...\n");
          UiStartLog();
          UiSetPruCtlBit(pru_mem, 0);

          UiPollPruCtlBit(pru_mem, 0, 0);
          UiStopAndSaveLog();

          trial_count = n;   // advance only after a completed trial
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- Set participant ID --------------------------------------------
        case 'p' : {
          printf("\t\tEnter participant ID: ");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %127s", participant);
          trial_count = 0;                 // reset numbering on participant change
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- Set condition [A/P] -------------------------------------------
        case 'c' : {
          char cin = 0;
          printf("\t\tEnter condition [A/P]: ");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %c", &cin);
          if (cin == 'a' || cin == 'A')      condition = 'A';
          else if (cin == 'p' || cin == 'P') condition = 'P';
          else printf("\t\tInvalid condition; keeping %c\n", condition);
          ApplyCondition(pru_mem);         // Passive -> Pd=0, Active -> assist level
          trial_count = 0;                 // reset numbering on condition change
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change Ptarget (assist level) ---------------------------------
        case 'a' : {
          printf("\t\tEnter new Ptarget (assist level): ");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %f", &input_float);
          active_ptarget = fix16_from_float(input_float);
          ApplyCondition(pru_mem);         // applies now if Active; stored if Passive
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change Tdelay -------------------------------------------------
        case 's' : {
          printf("\t\tEnter new Tdelay (ms): ");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Tdelay = (uint32_t)((float)input_float);
          UiPrintMenu(pru_mem);
          break;
        }

        // ---- change Ttrial -------------------------------------------------
        case 'd' : {
          printf("\t\tEnter new Ttrial (ms): ");
          fflush(stdout);
          UiPollForUserInput();
          scanf(" %f", &input_float);
          pru_mem->p->Ttrial = (uint32_t)((float)input_float);
          UiPrintMenu(pru_mem);
          break;
        }
      }
    }
  }
}

int PruLoadParams(const char* file, param_mem_t* param) {

  // Defaults
  param->fs_hz = 1000;
  param->fs_ticks = HZ_TO_TICKS(param->fs_hz);

  param->Ptarget = fix16_from_float(15.0);
  param->Tdelay = 10000;
  param->Ttrial = 30000;

  return 0;
}
