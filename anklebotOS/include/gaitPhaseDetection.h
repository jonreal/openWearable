#ifndef _GAITPHASEDETECTION_H_
#define _GAITPHASEDETECTION_H_

/* Global ------------------------------------------------------------------ */
extern volatile uint32_t *debugBuffer;

/* Structure ----------------------------------------------------------------*/
typedef struct{
  volatile uint16_t toe_hs;
  volatile uint16_t mid_hs;
  volatile uint16_t heel_hs;
  volatile uint16_t toe_to;
  volatile uint16_t mid_to;
  volatile uint16_t heel_to;

  volatile uint16_t prevGaitPhase;
  volatile uint16_t prevAvgPeriod;

  volatile uint16_t period[3];

  volatile uint8_t heel_indx;
  volatile uint8_t mid_indx;
  volatile uint8_t toe_indx;

} gp_t;

/* Prototypes ---------------------------------------------------------------*/
void gaitPhaseInit(param_mem_t* params);
void gaitPhaseUpdateParams(param_mem_t* params);
void gaitPhaseCleanUp(void);
void gaitPhaseDetect(volatile uint32_t cnt,
                     volatile uint16_t *gaitPhase,
                     volatile uint16_t *avgPeriod,
                     volatile uint16_t *adc);
uint16_t isHeelStrike(volatile uint16_t *adc);
uint16_t isToeOff(volatile uint16_t* adc);

#endif
