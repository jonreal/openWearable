#include "udf.h"

//
// Include sensor/actuator/algorithmic modules here
//
#include "pam.h"
#include "filter.h"
#include "filtcoeff.h"
#include "emg.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// resources
iir_filt_t* lp_1_5Hz_1;
iir_filt_t* lp_1_5Hz_2;
nlb_filt_t* nlb;
pam_t* pam;
emg_t* emg_sensor;

// Pam constants
const uint8_t div = 1;
const fix16_t threshold = FIX16_1;
const fix16_t p_min = 0xF0000; // 15 psi

// emg constants
const fix16_t k_emg_bias = 0x37A0000; // 890 bits
const fix16_t k_emg_scaling = 0x206;  // 4/507  (scaling/normalizing const)

// NLB constants
const fix16_t k_alpha = 0x42;     // 0.001
const fix16_t k_beta = 0x28F;     // 0.01


// Prototypes
fix16_t conditionEmg(fix16_t in);


// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
  emg_sensor = EmgInitSensor(0);
  nlb = FiltNlbInit(4,
                    fix16_from_int(1000),
                    fix16_one,
                    k_alpha,
                    k_beta);
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
  s_->emg_raw = EmgSample(emg_sensor);
  s_->emg_rect = conditionEmg(s_->emg_raw);
  s_->emg_nlb = FiltNlb(s_->emg_rect, nlb);
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
  pam = PamInitMuscle(SENSOR_ADD, MUX_SEL, 0, EXT_V_HP, EXT_V_LP, div,
                      threshold);
  lp_1_5Hz_1 = FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a);
  lp_1_5Hz_2 = FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a);

  mem->s->state->p_d = p_min;
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  PamSamplePressure(pam, &s_->p_m);
  s_->angle = s_->p_m;
  s_->p_m = FiltIir(s_->p_m,lp_1_5Hz_1);
  s_->dp_m = s_->p_m - FiltIir(s_->p_m,lp_1_5Hz_2);
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {

 // s_->p_d = s_->p_d - fix16_smul(K2,s_->dp_m);

 // if (s_->p_d < p_min)
 //   s_->p_d = p_min;
 //
  s_->p_d = p_min;
  PamUpdateControl(pam, &s_->p_m, &s_->p_d, &s_->u);
}

void Pru1Cleanup(void) {
  PamFreeMuscle(pam);
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
fix16_t conditionEmg(fix16_t in) {
  fix16_t out = fix16_ssub(in, k_emg_bias);
  // rectify
  if (out < 0)
   out = -out;
  return fix16_smul(k_emg_scaling, out);
}
