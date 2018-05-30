#include "udf.h"

// Include sensor/actuator/algorithmic here
#include "emg.h"
#include "pam.h"
#include "interaction.h"

// Define globals (scope is this module only)
volatile register uint32_t __R30;
pam_t* flx_pam;
pam_t* ext_pam;
interact_t* s1;
interact_t* s2;
iir_filt_t* lp_1_10Hz_1;
iir_filt_t* lp_1_10Hz_2;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {

  lp_1_10Hz_1 = IirInit(1, (fix16_t[]){0x7CD, 0x7CD},
                           (fix16_t[]){0x10000, 0xFFFF0F9A});
  lp_1_10Hz_2 = IirInit(1, (fix16_t[]){0x7CD, 0x7CD},
                           (fix16_t[]){0x10000, 0xFFFF0F9A});

  s1 = InteractionInit(0);
  s2 = InteractionInit(1);
}

void Pru0UpdateState(const pru_count_t* c, state_t* s_) {
  s_->inter_force = InteractionSampleForce(s1);
  s_->flx_strtch = IirFilt(InteractionSampleStretch(s2),lp_1_10Hz_1);
  s_->dflx_strtch = s_->flx_strtch - IirFilt(s_->flx_strtch,lp_1_10Hz_2);
}

void Pru0UpdateControl(const pru_count_t* c, state_t* s_) {
}

void Pru0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {
  flx_pam =
    PamInitMuscle(SENSOR_ADD, MUX_SEL, 1, FLEX_V_HP, FLEX_V_LP, 1, FIX16_1);
  ext_pam =
    PamInitMuscle(SENSOR_ADD, MUX_SEL, 0, EXT_V_HP, EXT_V_LP, 1, FIX16_1);
}

void Pru1UpdateState(const pru_count_t* c, state_t* s_) {

  debug_buff[2] = 0xAA;
  PamSamplePressure(flx_pam, &s_->flx_p);
  debug_buff[2] = 0xBB;
  __delay_cycles(1000);
  PamSamplePressure(ext_pam, &s_->ext_p);
  debug_buff[2] = 0x00;
}


void Pru1UpdateControl(const pru_count_t* c, state_t* s_) {

  s_->flx_pd = fix16_from_int(40);
  PamUpdateControl(flx_pam,
                   &s_->flx_p,
                   &s_->flx_pd,
                   &s_->flx_u);

  s_->ext_pd = fix16_from_int(1);
  PamUpdateControl(ext_pam,
                   &s_->ext_p,
                   &s_->ext_pd,
                   &s_->ext_u);

}

void Pru1Cleanup(void) {

  // Set muscle to 0 psi
  PamFreeMuscle(flx_pam);
  PamFreeMuscle(ext_pam);
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
