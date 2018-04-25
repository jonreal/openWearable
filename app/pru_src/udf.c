#include "udf.h"

// Include sensor/actuator/algorithmic here
#include "emg.h"
#include "pam.h"

// Define globals (scope is this module only)
volatile register uint32_t __R30;
pam_t* flx_pam;
pam_t* ext_pam;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
}

void Pru0UpdateState(const pru_count_t* c, pru_mem_t* mem) {

//  uint32_t bits = adcSampleCh(0);
//  mem->s->state[c->index].interaction_force =
//    fix16_sdiv(fix16_from_int((int16_t)bits - 2048),FORCEBIAS);
//
//  mem->s->state[c->index].flexion_pressure_d = fix16_from_int(10);
//  mem->s->state[c->index].extension_pressure_d = fix16_from_int(10);
//

}
void Pru0UpdateControl(const pru_count_t* c, pru_mem_t* mem) {
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

void Pru1UpdateState(const pru_count_t* c, pru_mem_t* mem) {
  PamSamplePressure(flx_pam, &mem->s->state[c->index].flexion_pressure);
  PamSamplePressure(ext_pam, &mem->s->state[c->index].extension_pressure);
}

void Pru1UpdateControl(const pru_count_t* c, pru_mem_t* mem) {


  volatile fix16_t pd = mem->l->
  debug_buff[1] = flx_pam->thr;

  PamUpdateControl(flx_pam,
                   &mem->s->state[c->index].flexion_pressure,
                   &pd,
                   &mem->s->state[c->index].flexion_cmd);

}

void Pru1Cleanup(void) {
  PamFreeMuscle(flx_pam);
  PamFreeMuscle(ext_pam);
}

// ---------------------------------------------------------------------------
// Added helper functions as needed
// ---------------------------------------------------------------------------
