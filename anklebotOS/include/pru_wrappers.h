#ifndef _PRU_WRAPPER_
#define _PRU_WRAPPER_

#define PRU_SENSOR    0
#define PRU_CONTROL   1

/* Globals ------------------------------------------------------------------*/
extern int debug;
extern FILE* fid;

/* Prototypes ---------------------------------------------------------------*/
void printDebugBuffer(void);
void initDebugBuffer(void);
int pru_run(const int pruNum, const char *const path);
int pru_init(void);
int pru_cleanup(void);
int pru_mem_init(void);
int armToPru0Interrupt(void);
int armToPru1Interrupt(void);
void writePruSensorParams(float frq_hz, uint32_t gp_thr_1,
                          uint32_t gp_thr_2, uint32_t gp_thr_3);
void writePruConrtolParams(uint32_t Kp, uint32_t Kd, uint32_t pos0,
                          uint32_t ff_traj[100]);
void writeState(uint8_t buffIndx);
void clearFlowBitFeild(void);
void enable(void);
void disable(void);
int isBufferFull(void);
void resetBufferFullFlag(void);
int whichBuffer(void);
uint32_t hzToPruTicks(float freq_hz);
float pruTicksToHz(uint32_t ticks);

#endif


