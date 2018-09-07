#include "udf.h"

// Include sensor/actuator/algorithmic modules here
#include "pam.h"
#include "sync.h"

// Define resources
volatile register uint32_t __R30;
volatile register uint32_t __R31;
pam_t* pam;
sync_t* sync;


uint32_t flag = 0;
uint32_t start_cnt = 0;
uint32_t trial_length = 5000;
const uint32_t num_cond = 5;
const uint32_t start_pressure = 5;


// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       state_t* s_,
                       pru_ctl_t* ctl_){
}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {
  pam = PamInitMuscle(SENSOR_ADD, MUX_SEL, 0, EXT_V_HP, EXT_V_LP, 1, FIX16_1);
  sync = SyncInitChan(12);
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
  PamSamplePressure(pam, &s_->p_m);
  s_->sync_val = SyncOutState(sync);
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {

  s_->p_d = fix16_from_int(0);

  if (ctl_->bit.utility == 1) {

    if (flag == 0) {
      flag = 1;
      start_cnt = c->frame;
      SyncOutHigh(sync);
    }

    s_->p_d = fix16_from_int( 5 * ((c->frame - start_cnt) / trial_length));

    if (c->frame >= (start_cnt + 6 * trial_length)) {
      ctl_->bit.utility = 0;
      SyncOutLow(sync);
    }
  }

  PamUpdateControl(pam,
                   &s_->p_m,
                   &s_->p_d,
                   &s_->u);
}

void Pru1Cleanup(void) {
  PamFreeMuscle(pam);
  SyncFreeChan(sync);
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
