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


// ---- static functions -----------------------------------------------------
void PamUpdateControl(pam_t* pam) {
  switch (pam->u) {
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
pam_t* PamInitMuscle(pressure_sensor_t* sensor,
                     uint32_t in_pin,
                     uint32_t out_pin) {

  pam_t* pam = malloc(sizeof(pam_t));
  pam->sensor = sensor;
  pam->hp_pin = in_pin;
  pam->lp_pin = out_pin;
  pam->u = 0;
  pam->p_m = 0;
  pam->p_d = 0;
  pam->p_max = 0;
  pam->reflex = 0;
  pam->flag = 0;

  // Will hang here if i2c mux/channel is off
  PamUpdateSensors(pam,NULL);
  return pam;
}

reservoir_t* PamInitReservoir(pressure_sensor_t* sensor) {

  reservoir_t* reservoir = malloc(sizeof(reservoir_t));
  reservoir->sensor = sensor;

  // Will hang here if i2c mux/channel is off
  PamSampleReservoirPressure(reservoir);
  return reservoir;
}

void PamMuscleFree(pam_t* pam) {
   __R30 |= (1 << pam->lp_pin);
   __R30 &= ~(1 << pam->hp_pin);
  PressureSensorFree(pam->sensor);
  free(pam);
}

void PamReservoirFree(reservoir_t* reservoir) {
  PressureSensorFree(reservoir->sensor);
  free(reservoir);
}

void PamUpdateSensors(pam_t* pam, iir_filt_t* f){
  if(f)
    pam->p_m = FiltIir(PressureSensorSample(pam->sensor),f);
  else
    pam->p_m = PressureSensorSample(pam->sensor);
}

void PamSetU(pam_t* pam, int8_t u) {
  pam->u = u;
}

fix16_t PamGetPm(pam_t* pam) {
  return pam->p_m;
}

fix16_t PamGetPd(pam_t* pam) {
  return pam->p_d;
}

fix16_t PamGetPmax(pam_t* pam) {
  return pam->p_max;
}


void PamSetPd(pam_t* pam, fix16_t Pd) {
  pam->flag = 0;
  pam->p_d = Pd;
}

fix16_t PamSampleReservoirPressure(const reservoir_t* reservoir){
  return PressureSensorSample(reservoir->sensor);
}

void PamSimple(pam_t* p) {

  // update sensors
  PamUpdateSensors(p,NULL);

  // reached goal flag
  if (fix16_ssub(p->p_d,fix16_ssub(p->p_m,0x20000)) < 0)
    p->flag = 1;

  // if pd = 0 open exhaust
  if (p->p_d == 0)
    PamSetU(p,-1);
  else if ((fix16_ssub(p->p_d,fix16_ssub(p->p_m,0x20000)) > 0) && (p->flag==0))
    PamSetU(p,1);
  else
    PamSetU(p,0);

  PamUpdateControl(p);
}



void PamAntagonistReflex(pam_t* p1, pam_t* p2,
                        iir_filt_t* f1, iir_filt_t* f2,
                        uint8_t period,  uint8_t pulsewidth,
                        uint8_t pulseonset) {

  static volatile uint8_t cnt1 = 0;
  static volatile uint8_t type1 = 0;
  static volatile uint8_t cnt2 = 0;
  static volatile uint8_t type2 = 0;
  static volatile uint8_t flag2 = 0;
  static volatile uint8_t flag1 = 0;
  fix16_t dp = 0x20000; // 0.5

  const fix16_t dp_max = fix16_from_int(10);
  const fix16_t dp_min = fix16_from_int(10);

  PamUpdateSensors(p1,f1);
  PamUpdateSensors(p2,f2);

  // Determine which reflex
  if ((type1 == 0) && (fix16_ssub(p1->p_d,p1->p_m) > fix16_from_int(20))) {
    type1 = 1;
  } else if ((type1 == 0) && (p1->p_m < fix16_ssub(p1->p_d,dp_min))) {
    type1 = 2;
  } else if ((type1 == 0) && (type2 == 0) &&
      (p1->p_m > fix16_sadd(p1->p_d,dp_max))) {
    type1 = 3;
  }

  // Determine which reflex
  if ((type2 == 0) && (fix16_ssub(p2->p_d,p2->p_m) > fix16_from_int(20))) {
    type2 = 1;
  } else if ((type2 == 0) && (p2->p_m < fix16_ssub(p2->p_d,dp_min))) {
    type2 = 2;
  } else if ((type1 == 0) && (type2 == 0) &&
      (p2->p_m > fix16_sadd(p2->p_d,dp_max))) {
    type2 = 3;
  }


  if (type1 == 1) {
    if (p1->p_m > p1->p_d){
      cnt1 = 0;
      flag1 = 1;
      PamSetU(p1,0);
    } else if ((flag1) && (cnt1 == period)) {
      type1 = 0;
      flag1 = 0;
      cnt1 = 0;
      p1->p_max = p1->p_m;
    } else if (!flag1) {
      PamSetU(p1,1);
      cnt1 = 0;
    }
    cnt1++;
  }

  if (type1 == 2) {
    if (cnt1 == period) {
      type1 = 0;
      cnt1 = 0;
      PamSetU(p1,0);
      p1->p_max = p1->p_m;
    } else if ((cnt1 >= pulseonset) && (cnt1 <= (pulseonset + pulsewidth))) {
      PamSetU(p1,1);
    } else {
      PamSetU(p1,0);
    }
    cnt1++;
  }

  if (type1 == 3) {
    if (cnt1 == period) {
      type1 = 0;
      cnt1 = 0;
      PamSetU(p1,0);
      //PamSetU(p2,0);
      p2->p_d = p2->p_d + dp;
      p1->p_d = p1->p_d - dp;
      p1->p_max = p1->p_m;
      p2->p_max = p2->p_m;
    } else if ((cnt1 >= pulseonset) && (cnt1 <= (pulseonset + 8*pulsewidth))) {
      PamSetU(p1,-1);
      //if ((cnt1 >= pulseonset) && (cnt1 <= (pulseonset + pulsewidth))) {
      //  PamSetU(p2,1);
      //} else {
      //  PamSetU(p2,0);
      //}
    } else {
      PamSetU(p1,0);
      PamSetU(p2,0);
    }
    cnt1++;
  }

//  if (type1 == 3) {
//    if (cnt1 == period) {
//      type1 = 0;
//      cnt1 = 0;
//      PamSetU(p1,0);
//    //  PamSetU(p2,0);
//      p2->p_d = p2->p_d + dp;
//      p1->p_d = p1->p_d - dp;
//    } else if ((cnt1 >= pulseonset) && (cnt1 <= (pulseonset + 4*pulsewidth))) {
//      PamSetU(p1,-1);
//   //   PamSetU(p2,1);
//    } else {
//      PamSetU(p1,0);
//      PamSetU(p2,0);
//    }
//    cnt1++;
//  }


  if (type2 == 1) {
    if (p2->p_m > p2->p_d){
      cnt2 = 0;
      flag2 = 1;
      PamSetU(p2,0);
    } else if ((flag2) && (cnt2 == period)) {
      type2 = 0;
      flag2 = 0;
      p2->p_max = p2->p_m;
      cnt2 = 0;
    } else if (!flag2) {
      PamSetU(p2,1);
      cnt2 = 0;
    }
    cnt2++;
  }


  if (type2 == 2) {
    if (cnt2 == period) {
      type2 = 0;
      cnt2 = 0;
      p2->p_max = p2->p_m;
      PamSetU(p2,0);
    } else if ((cnt2 >= pulseonset) && (cnt2 <= (pulseonset + pulsewidth))) {
      PamSetU(p2,1);
    } else {
      PamSetU(p2,0);
    }
    cnt2++;
  }

  if (type2 == 3) {
    if (cnt2 == period) {
      type2 = 0;
      cnt2 = 0;
      PamSetU(p2,0);
      //PamSetU(p1,0);
      p2->p_d = p2->p_d - dp;
      p1->p_d = p1->p_d + dp;
    } else if ((cnt2 >= pulseonset) && (cnt2 <= (pulseonset + 8*pulsewidth))) {
      PamSetU(p2,-1);
      //if ((cnt1 >= pulseonset) && (cnt1 <= (pulseonset + pulsewidth))) {
      //  PamSetU(p1,1);
      //} else {
      //  PamSetU(p1,0);
      //}
    } else {
      PamSetU(p2,0);
      PamSetU(p1,0);
    }
    cnt2++;
  }

  PamUpdateControl(p2);
  PamUpdateControl(p1);
  p1->reflex = type1;
  p2->reflex = type2;
}


void PamReflex(pam_t* pam, iir_filt_t* filt,
                        uint8_t period,  uint8_t pulsewidth,
                        uint8_t pulseonset) {

  static volatile uint8_t cnt1 = 0;
  static volatile uint8_t type = 0;
  static volatile uint8_t flag1 = 0;
  const fix16_t dp = 0x4000;
  const fix16_t dp_max = fix16_from_int(0.5);
  const fix16_t dp_min = fix16_from_int(1);

  PamUpdateSensors(pam,NULL);

  // Determine which reflex
  if ((type == 0) && (fix16_ssub(pam->p_d,pam->p_m) > fix16_from_int(20))) {
    type = 1;
  } else if ((type == 0) && (pam->p_m < fix16_ssub(pam->p_d,dp_min))) {
    type = 2;
  } else if ((type == 0) && (pam->p_m > fix16_sadd(pam->p_d,dp_max))) {
    type = 3;
  }

  // Execute reflex
  if (type == 1) {
    if (pam->p_m > pam->p_d){
      cnt1 = 0;
      flag1 = 1;
      PamSetU(pam,0);
    } else if ((flag1) && (cnt1 == period)) {
      type = 0;
      flag1 = 0;
      cnt1 = 0;
    } else if (!flag1) {
      PamSetU(pam,1);
      cnt1 = 0;
    }
    cnt1++;
  }

  if (type == 2) {
    if (cnt1 == period) {
      type = 0;
      cnt1 = 0;
      PamSetU(pam,0);
    } else if ((cnt1 >= pulseonset) && (cnt1 <= (pulseonset + pulsewidth))) {
      PamSetU(pam,1);
    } else {
      PamSetU(pam,0);
    }
    cnt1++;
  }

  if (type == 3) {
    if (cnt1 == period) {
      type = 0;
      cnt1 = 0;
      PamSetU(pam,0);
      pam->p_d = pam->p_d - dp;
      if ((pam->p_d) < fix16_from_int(10))
        pam->p_d = fix16_from_int(10);
    } else if ((cnt1 >= pulseonset) && (cnt1 <= (pulseonset + 2*pulsewidth))) {
      PamSetU(pam,-1);
    } else {
      PamSetU(pam,0);
    }
    cnt1++;
  }
  PamUpdateControl(pam);
  pam->reflex = type;
}






void PamMyReflex(pam_t* pam, uint8_t period,  uint8_t pulsewidth,
                  uint8_t pulseonset) {

  static volatile uint8_t cnt = 0;
  static volatile uint8_t type = 0;

  fix16_t e = fix16_ssub(pam->p_d, pam->p_m);
  fix16_t dp = 0x4000; // 0.25


  // Determine which reflex
  if ((type == 0) && (e > 0)) {
    type = 1;
  } else if ((type == 0) && (e < fix16_from_int(-4))) {
    type = 2;
  }

  if (type == 1) {
    if (cnt == period) {
      type = 0;
      cnt = 0;
      PamSetU(pam,0);
    } else if ((cnt >= pulseonset) && (cnt <= (pulseonset + pulsewidth))) {
      PamSetU(pam,1);
    } else if ((cnt < pulseonset) && (cnt > (pulseonset + pulsewidth))) {
      PamSetU(pam,0);
    }
    cnt++;
  }

  if (type == 2) {
    if (cnt == period) {
      type = 0;
      cnt = 0;
      PamSetU(pam,0);
      PamSetPd(pam,fix16_ssub(PamGetPd(pam),dp));
    } else if ((cnt >= pulseonset) && (cnt <= (pulseonset + pulsewidth))) {
      PamSetU(pam,-1);
    } else if ((cnt < pulseonset) && (cnt > (pulseonset + pulsewidth))) {
      PamSetU(pam,0);
    }
    cnt++;
  }

  PamUpdateControl(pam);
}




int16_t PamGetU(pam_t* pam) {
  return pam->u;
}


reflex_t* PamInitReflex(uint8_t pw, uint8_t T, uint8_t po, int8_t sign,
                        fix16_t thresh) {

  reflex_t* reflex = malloc(sizeof(reflex_t));

  reflex->pulsewidth = pw;
  reflex->period = T;
  reflex->pulseonset = po;
  reflex->sign = sign;
  reflex->threshold = thresh;
  reflex->cnt = 0;
  reflex->on = 0;

  return reflex;
}
