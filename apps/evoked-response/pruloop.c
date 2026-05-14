/* Copyright 2017-2019 Jonathan Realmuto

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
=============================================================================*/

#include "pruloop.h"
#include "hscsann150pg2a3.h"
#include "sync.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Pam
i2cmux_t* mux;
i2c_t* i2c;
pressure_sensor_t* pressure_sensor;

// Sync In
sync_t* sync;

uint32_t log_flag = 0;

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {
}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
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

  i2c = I2cInit(2);
  mux = MuxI2cInit(i2c,0x70,PCA9548);

  // pam -- only sensor for now
  // sensor on mux ch. 6, 6 - 1 = 5
  pressure_sensor = PressureSensorInit(mux,5,0x28);

  // Sync Out ---- NEED TO CHANGE TO PROPER PIN
  // In: P8.42, MODE6, pr1_pru1_pru_r31_5 */
  sync = SyncInitChan(5);

}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {


  uint32_t ctl_bit = PruGetCtlBit(ctl_,0);
  if (ctl_bit && !log_flag) {
    log_flag = 1;
    SyncOutHigh(sync);
  } else if (log_flag && !ctl_bit) {
    log_flag = 0;
    SyncOutLow(sync);
  }

  s_->pressure = PressureSensorSample(pressure_sensor);
  s_->sync_state = SyncGetState(sync);
}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
}

void Pru1Cleanup(void) {
  PressureSensorFree(pressure_sensor);
}

