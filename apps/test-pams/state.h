#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "../../library/pru/include/pam.h"   // pam_state_t (+ pressure/mux types) by relative path

// packing can be issue -- use 32bit types

// --- NN inference IO --------------------------------------------------------
// Kept because the shared a72 core (nn.c) always links the Nn* app hooks and
// writes cpu_state.nn. NN stays dormant unless launched with -n <net> -i <io>.
#define N_FEAT 16   // model input  width
#define N_OUT  8    // model output width

typedef struct {
  float x[N_FEAT];
} nn_feat_t;

typedef struct {
  volatile fix16_t  y[N_OUT];
  volatile uint32_t seq;
  volatile uint32_t stamp;
  volatile int32_t  status;
} nn_out_t;

// --- cpu_state struct
typedef struct {
  volatile uint32_t cpuvar;
  nn_out_t nn;
} cpu_state_t;

// --- r5f_state: the R5F pair's output, snapshotted into the ring by PRU0.
typedef struct {
  volatile uint32_t r5fvar;
} r5f_state_t;

// --- State
typedef struct {
  volatile uint32_t frame;          // monotonic loop counter (PRU0 writes pru_count_t.frame)
  volatile uint32_t pru0var;
  volatile uint32_t pru1var;
  volatile fix16_t  p_res;          // reservoir pressure (PRU1, I2C)
  volatile pam_state_t pam_state;   // PAM muscle state: pd / pm / pm_raw / u
  cpu_state_t cpu_state;
  r5f_state_t r5f_state;
 } state_t;

// --- Parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  uint32_t r5f_decimate;   // R5F hooks every Nth PRU frame (A72-set; no fw rebuild to retune)
  fix16_t  Pd;             // desired muscle pressure (UI-changeable)
} param_mem_t;


// -- Shared Memory layout -- must match .cmd files
static const uintptr_t global_sharedram_base = 0x0b010000;
static const uintptr_t sharedram_base = 0x00010000;

static const uintptr_t state_ram_offset = 0x0;
static const uintptr_t param_ram_offset = 0xE000;
static const uintptr_t lut_ram_offset = 0xF000;

#endif
