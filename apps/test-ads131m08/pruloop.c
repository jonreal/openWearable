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
#include "pwmdriver.h"
#include "spidriver.h"


#define RREG(aa, nn) (0b101 << 13) | ((aa & 0x3F) << 7) | (nn & 0x7F)

volatile register uint32_t __R30;
volatile register uint32_t __R31;

uint32_t cs_pin = 6; /* P9.27 */

// ---------------------------------------------------------------------------
// PRU0
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {
  //pwmInit();
  //pwmSetDutyCycle(fix16_from_int(50));

  // High 
  __R30 |= (1 << cs_pin);
  spiInit();
}

void Pru0UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

  uint32_t rx[10];
  //uint32_t cmd = 0xA000;
  //uint32_t cmd = 0b101000000000000;
  //uint32_t cmd = 0x0;
  uint32_t cmd = RREG(0x0,0x0);

  // Issue command first, need 10 xfer per transaction
  //spiForceBeginXfer();

  __R30 &= ~(1 << cs_pin);

  //rx[0] = spiXfer(0, cmd);
  //for (int i=1; i<10; i++) {
  //  rx[i] = spiXfer(0, 0x00);
  //};

  uint32_t tx[10] = {cmd, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  spiMultiWordXfer(tx, rx, 10);

  __R30 |= (1 << cs_pin);
  //spiForceEndXfer();

 //spiForceBeginXfer();
 //for (int i=0; i<10; ++i) {
 //  rx[i] = spiXfer(0, 0x00);
 //  __delay_cycles(100);
 //}
 //spiForceEndXfer();

  s_->rx0 = rx[0];
  s_->rx1 = rx[1];
  s_->rx2 = rx[2];
  s_->rx3 = rx[3];
  s_->rx4 = rx[4];
  s_->rx5 = rx[5];
  s_->rx6 = rx[6];
  s_->rx7 = rx[7];
  s_->rx8 = rx[8];
  s_->rx9 = rx[9];

//  s_->rx0 = rx[0] & 0xFFFF;
//  s_->rx1 = rx[1] & 0xFFFFFF;
//  s_->rx2 = rx[2] & 0xFFFFFF;
//  s_->rx3 = rx[3] & 0xFFFFFF;
//  s_->rx4 = rx[4] & 0xFFFFFF;
//  s_->rx5 = rx[5] & 0xFFFFFF;
//  s_->rx6 = rx[6] & 0xFFFFFF;
//  s_->rx7 = rx[7] & 0xFFFFFF;
//  s_->rx8 = rx[8] & 0xFFFFFF;
//  s_->rx9 = rx[9] & 0xFFFFFF;



}

void Pru0UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_){
  debug_buff[0] = 0xFF;
}

void Pru0Cleanup(void) {
 // pwmCleanUp();
}

// ---------------------------------------------------------------------------
// PRU1
//
// Edit user defined functions below
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {
}

void Pru1UpdateState(const pru_count_t* c,
                     const param_mem_t* p_,
                     const lut_mem_t* l_,
                     state_t* s_,
                     pru_ctl_t* ctl_) {

}

void Pru1UpdateControl(const pru_count_t* c,
                       const param_mem_t* p_,
                       const lut_mem_t* l_,
                       state_t* s_,
                       pru_ctl_t* ctl_) {
}

void Pru1Cleanup(void) {
}

