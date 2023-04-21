/* Copyright 2018 Jonathan Realmuto

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


#include "pam.h"
#include <stdlib.h>

volatile register uint32_t __R30;
extern volatile uint32_t *debug_buff;

uint32_t countingL = 0;
uint32_t countingR = 0;
uint32_t time_calL = 0;
uint32_t time_calR = 0;
uint32_t waittime = 1000;
fix16_t ten_three = 0x3E80000;
fix16_t ten_two = 0x640000;
fix16_t ten_one = 0xA0000;
fix16_t Rcoe1 = 0x4F8AF; //4.9714285714
fix16_t Rcoe2 = 0x05D8C; //0.3654285714
fix16_t coe5_num = 0x5C340; //5.7627
fix16_t coe4_num = -0x74BB9; //-7.2958
fix16_t coe3_num = 0x4BDB6; //4.74107
fix16_t coe2_num = -0x28D88; //-2.55286
fix16_t coe1_num = 0x20EF9; //2.0585
fix16_t coe0_num = -0x38119; //-3.5043
fix16_t Pt_model = 0x690000; //model tank pressure is 105 psi
fix16_t Pm0_model = 0x00000; //model initial muscle pressure is 0 psi

// ---- static functions -----------------------------------------------------
static void PamUpdateControl(pam_t* pam) {
  switch (pam->s.u) {
    case 0:
      __R30 &= ~(1 << pam->hp_pin) & ~(1 << pam->lp_pin);
      break;
    case 1:
      __R30 |= (1 << pam->hp_pin);
      __R30 &= ~(1 << pam->lp_pin);
      break;
    case -1:
      __R30 |= (1 << pam->lp_pin);
      __R30 &= ~(1 << pam->hp_pin);
      break;
  }
}


// ---------------------------------------------------------------------------
reservoir_t* PamReservoirInit(pressure_sensor_t* sensor) {
  reservoir_t* reservoir = malloc(sizeof(reservoir_t));
  reservoir->sensor = sensor;
  PamReservoirUpdate(reservoir);
  return reservoir;
}

void PamReservoirFree(reservoir_t* reservoir) {
  PressureSensorFree(reservoir->sensor);
  free(reservoir);
}

void PamReservoirUpdate(reservoir_t* reservoir){
  reservoir->pressure = PressureSensorSample(reservoir->sensor);
}

fix16_t PamReservoirGetPressure(const reservoir_t* reservoir) {
  return reservoir->pressure;
}


// ---------------------------------------------------------------------------
pam_t* PamInitMuscle(pressure_sensor_t* sens,
                      reservoir_t* reservoir,
                      uint32_t in_pin,
                      uint32_t out_pin,
                      fix16_t tau_in,
                      fix16_t tau_out,
                      uint32_t refract,
                      iir_filt_t* filter){
  pam_t* pam = malloc(sizeof(pam_t));
  pam->sensor = sens;
  pam->res = reservoir;
  pam->hp_pin = in_pin;
  pam->lp_pin = out_pin;
  pam->tau_in = tau_in;
  pam->tau_out = tau_out;
  pam->T_refract = refract;
  pam->fsm = HOLD;
  pam->s.u = 0;
  pam->s.pm = 0;
  pam->s.pd = 0;
  pam->filt = filter;
  pam->pcnt = 0;
  pam->pw = 0;

  // Will hang here if i2c mux/channel is off
  PamUpdate(pam);
  return pam;
}

void PamMuscleFree(pam_t* pam) {
   __R30 |= (1 << pam->lp_pin);
   __R30 &= ~(1 << pam->hp_pin);
  PressureSensorFree(pam->sensor);
  free(pam);
}

pam_state_t PamGetState(const pam_t* pam) {
  return pam->s;
}

void PamUpdate(pam_t* pam) {
  pam->s.pm_raw = PressureSensorSample(pam->sensor);
  if (pam->filt)
    pam->s.pm = FiltIir(pam->s.pm_raw,pam->filt);
  else
    pam->s.pm = pam->s.pm_raw;
}

void PamSetU(pam_t* pam, int8_t u) {
  pam->s.u = u;
}

void PamSetPd(pam_t* pam, fix16_t Pd) {

  if (Pd == 0)
    pam->fsm = ZERO;

  if ((pam->fsm == HOLD) || (pam->fsm == ZERO)){
    if (Pd == 0) {
      pam->fsm = ZERO;
    } else if (Pd < pam->s.pm) {
      pam->fsm = DEFLATE;
      pam->s.pd = Pd;
      //pam->pw = (uint32_t)
      //  (-fix16_mul(pam->tau_out,fix16_log(fix16_div(Pd,pam->s.pm))));
    } else if (Pd > pam->s.pm) {
      pam->fsm = INFLATE;
      pam->s.pd = Pd;
      //pam->pw = (uint32_t)
      //  (-fix16_mul(pam->tau_out,
      //    fix16_log(fix16_div(fix16_sub(Pd,pam->res->pressure),
      //              fix16_sub(pam->s.pm,pam->res->pressure)))));
    }
  }
}

void PamActionSimple(pam_t* p) {
  switch (p->fsm) {
    case ZERO : {
      p->fsm = ZERO;
      PamSetU(p,-1);
//      PamSetU(p,-1);
      break;
    }
    case INFLATE : {
      if (fix16_sub(p->s.pd,p->s.pm) < 0) {
        p->fsm = REFRACT;
        PamSetU(p,0);
      } else {
          PamSetU(p,1);
      }
      break;
    }
    case DEFLATE : {
      if (fix16_sub(p->s.pd,p->s.pm) > 0) {
        p->fsm = REFRACT;
        PamSetU(p,0);
      } else {
        PamSetU(p,-1);
      }
      break;
    }
    case REFRACT : {
      if (p->cnt == p->T_refract) {
        p->fsm = HOLD;
        p->cnt = 0;
      } else {
        p->cnt++;
      }
      break;
    }
  }
  PamUpdateControl(p);
}

void PamActionSimpleXWL(pam_t* pL) {
  switch (pL->fsm) {
    case ZERO : {
      pL->fsm = ZERO;
      PamSetU(pL,-1);
      break;
    }
    case INFLATE : {
      if (countingL == 0) {
        fix16_t PtL = PamReservoirGetPressure(pL->res);
	      fix16_t Pm0L = pL->s.pm;
	      fix16_t Pd_modelL = fix16_mul(fix16_div(fix16_sub(Pt_model,Pm0_model),fix16_sub(PtL,Pm0L)),pL->s.pd);
	      fix16_t PratioL = fix16_add(fix16_mul(fix16_div(Rcoe1,ten_three),Pd_modelL),Rcoe2);
	      fix16_t Pd_outputL = fix16_div(fix16_mul(PratioL,Pd_modelL),ten_one);
      	fix16_t output2L = fix16_mul(Pd_outputL,Pd_outputL);
	      fix16_t output3L = fix16_mul(Pd_outputL,output2L);
	      fix16_t output4L = fix16_mul(output2L,output2L);
	      fix16_t output5L = fix16_mul (output2L,output3L);
	      fix16_t time5L = fix16_mul(fix16_div(coe5_num,ten_three),output5L);
      	fix16_t time4L = fix16_mul(fix16_div(coe4_num,ten_two),output4L);
      	fix16_t time3L = fix16_mul(fix16_div(coe3_num,ten_one),output3L);
      	fix16_t time2L = fix16_mul(fix16_div(coe2_num,ten_two),output2L);
      	fix16_t time1L = fix16_mul(fix16_mul(coe1_num,ten_one),Pd_outputL);
      	fix16_t time0L = fix16_div(coe0_num,ten_one);
      	fix16_t Adj_timeL = fix16_add(fix16_add(fix16_add(fix16_add(fix16_add(
                    time5L,time4L),time3L),time2L),time1L),time0L);
	      time_calL = fix16_to_int(Adj_timeL);
      }
      if (fix16_sub(pL->s.pd,pL->s.pm) < 0) {
	      pL->fsm = REFRACT;
      	PamSetU(pL,0);
      } else {
      	countingL++;
      	if (countingL < waittime) {
      	  PamSetU(pL,0) ;
      	} else if (countingL >= waittime) {
    	    if (countingL <= (waittime+time_calL)) {
      	    PamSetU(pL,1);
    	    } else {
      	    PamSetU(pL,0);
  	      }
    	  }
      }
	  break;
    }
    case DEFLATE : {
      if (fix16_sub(fix16_add(pL->s.pd,0x50000),pL->s.pm) > 0) {
      	pL->fsm = REFRACT;
	      PamSetU(pL,0);
      } else {
      	PamSetU(pL,-1);
      }
    break;
    }
    case REFRACT : {
      if (pL->cnt == pL->T_refract) {
	      pL->fsm = HOLD;
      	pL->cnt = 0;
      } else {
      	pL->cnt++;
      }
    break;
    }
  }
PamUpdateControl(pL);
}


void PamActionSimpleXWR(pam_t* pR) {
  switch (pR->fsm) {
    case ZERO : {
      pR->fsm = ZERO;
      PamSetU(pR,-1);
      break;
    }
    case INFLATE : {
      if (countingR == 0) {
        fix16_t Pt = PamReservoirGetPressure(pR->res);
	      fix16_t Pm0 = pR->s.pm;
	      fix16_t Pd_model = fix16_mul(fix16_div(fix16_sub(Pt_model,Pm0_model),fix16_sub(Pt,Pm0)),pR->s.pd);
	      fix16_t Pratio = fix16_add(fix16_mul(fix16_div(Rcoe1,ten_three),Pd_model),Rcoe2);
	      fix16_t Pd_output = fix16_div(fix16_mul(Pratio,Pd_model),ten_one);
      	fix16_t output2 = fix16_mul(Pd_output,Pd_output);
	      fix16_t output3 = fix16_mul(Pd_output,output2);
	      fix16_t output4 = fix16_mul(output2,output2);
	      fix16_t output5 = fix16_mul (output2,output3);
	      fix16_t time5 = fix16_mul(fix16_div(coe5_num,ten_three),output5);
      	fix16_t time4 = fix16_mul(fix16_div(coe4_num,ten_two),output4);
      	fix16_t time3 = fix16_mul(fix16_div(coe3_num,ten_one),output3);
      	fix16_t time2 = fix16_mul(fix16_div(coe2_num,ten_two),output2);
      	fix16_t time1 = fix16_mul(fix16_mul(coe1_num,ten_one),Pd_output);
      	fix16_t time0 = fix16_div(coe0_num,ten_one);
      	fix16_t Adj_timeR = fix16_add(fix16_add(fix16_add(fix16_add(fix16_add(
                    time5,time4),time3),time2),time1),time0);
	      time_calR = fix16_to_int(Adj_timeR);
      }
      if (fix16_sub(pR->s.pd,pR->s.pm) < 0) {
	      pR->fsm = REFRACT;
      	PamSetU(pR,0) ;
      } else {
      	countingR++;
      	if (countingR < waittime) {
      	  PamSetU(pR,0) ;
      	} else if (countingR >= waittime) {
    	    if (countingR <= (waittime+time_calR)) {
      	    PamSetU(pR,1);
    	    } else {
      	    PamSetU(pR,0);
  	      }
    	  }
      }
	  break;
    }
    case DEFLATE : {
      if (fix16_sub(fix16_add(pR->s.pd,0x50000),pR->s.pm) > 0) {
      	pR->fsm = REFRACT;
	      PamSetU(pR,0);
      } else {
      	PamSetU(pR,-1);
      }
    break;
    }
    case REFRACT : {
      if (pR->cnt == pR->T_refract) {
	      pR->fsm = HOLD;
      	pR->cnt = 0;
      } else {
      	pR->cnt++;
      }
    break;
    }
  }
PamUpdateControl(pR);
}


void PamActionModel(pam_t* p) {

  switch (p->fsm) {
    case INFLATE : {
      if (p->pcnt < p->pw) {
        PamSetU(p,1);
      }
      else {
        PamSetU(p,0);
        p->fsm = REFRACT;
        p->pcnt = 0;
      }
      p->pcnt++;
      break;
    }
    case DEFLATE : {
      if (p->pcnt < p->pw) {
        PamSetU(p,-1);
      }
      else {
        PamSetU(p,0);
        p->fsm = REFRACT;
        p->pcnt = 0;
      }
      p->pcnt++;
      break;
    }
    case REFRACT : {
      if (p->cnt == p->T_refract) {
        p->fsm = HOLD;
        p->cnt = 0;
      } else {
        p->cnt++;
      }
      break;
    }
  }
  PamUpdateControl(p);
}

void PamActionPulse(pam_t* p) {
  static const uint32_t pulse_width = 5;
  static const uint32_t pulse_period = 50;

  switch (p->fsm) {
    case INFLATE : {
      if (fix16_sub(p->s.pd,p->s.pm) < 0) {
        p->fsm = REFRACT;
        p->pcnt = 0;
        PamSetU(p,0);
      } else {
        if ((p->pcnt%pulse_period) < pulse_width) {
          PamSetU(p,1);
        }
        else {
          PamSetU(p,0);
        }
      }
      p->pcnt++;
      break;
    }
    case DEFLATE : {
      if (fix16_sub(p->s.pd,p->s.pm) > 0) {
        p->fsm = REFRACT;
        p->pcnt = 0;
        PamSetU(p,0);
      } else {
        if ((p->pcnt%pulse_period) < pulse_width) {
          PamSetU(p,-1);
        }
        else {
          PamSetU(p,0);
        }
      }
      p->pcnt++;
      break;
    }
    case REFRACT : {
      if (p->cnt == p->T_refract) {
        p->fsm = HOLD;
        p->cnt = 0;
      } else {
        p->cnt++;
      }
      break;
    }
  }
  PamUpdateControl(p);
}
