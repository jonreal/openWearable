#ifndef _STATE_
#define _STATE_

#include "fix16.h"
#include "maxon.h"
#include "pam.h"
#include "emg.h"
#include "bno055_arm.h"

// --- cpudata struct
typedef struct {
  volatile euler_t imu1;
  volatile uint32_t status;
} cpudata_t;

// --- state
typedef struct {
  volatile uint32_t time;
  cpudata_t cpudata;
  volatile fix16_t x;
  volatile fix16_t xd;
  volatile uint32_t vsync;
  volatile motor_state_t motor;
  volatile fix16_t dx;
  volatile fix16_t tau_active;
  volatile fix16_t p_res;
  volatile pam_state_t pam1_state;
  volatile pam_state_t pam2_state;
  volatile fix16_t triggersignal;
  volatile fix16_t triggersignal_myo;
  volatile emg_state_t emg1_state;
  volatile emg_state_t emg2_state;
  volatile uint32_t game;
 } state_t;

// --- parameters
typedef struct {
  volatile uint32_t debug_buff[10];
  uint32_t fs_hz;
  uint32_t fs_ticks;
  fix16_t targets[20];
  fix16_t Jvirtual;
  fix16_t bvirtual;
  fix16_t kvirtual;
  fix16_t G;
  fix16_t P0;
  fix16_t threshold;
  fix16_t dP;
  fix16_t emg_up_thres;
  fix16_t emg_low_thres;
  uint32_t reflex_condition;
  uint32_t Nb;
  fix16_t Np;
  fix16_t fd;
  fix16_t Td;
  fix16_t k2PiFd;
} param_mem_t;


#endif
