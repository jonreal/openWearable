#ifndef _PRU_WRAPPER_
#define _PRU_WRAPPER_

#define PRU_SENSOR    0
#define PRU_CONTROL   1

/* Globals ------------------------------------------------------------------*/
extern int debug;

/* Local Globals- */
FILE* fid;
uint32_t collectionFlag;

/* Prototypes ---------------------------------------------------------------*/
void printDebugBuffer(void);
void initDebugBuffer(void);
int pru_run(const int pruNum, const char *const path);
int pru_init(void);
int pru_cleanup(void);
int pru_mem_init(void);
int armToPru0Interrupt(void);
int armToPru1Interrupt(void);
//void writePruSensorParams(float frq_hz, uint32_t gp_thr_1,
//                          uint32_t gp_thr_2, uint32_t gp_thr_3);

void writePruParams(float frq_hz,
                    uint16_t toe_hs,
                    uint16_t mid_hs,
                    uint16_t heel_hs,
                    uint16_t toe_to,
                    uint16_t mid_to,
                    uint16_t heel_to,
                    uint16_t gpOnLeftFoot,
                    uint16_t Kp,
                    uint16_t Kd,
                    int16_t anklePos0);

void writePruConrtolParams(uint32_t Kp, uint32_t Kd, uint32_t pos0,
                          uint32_t ff_traj[100]);
void writeState(uint8_t buffIndx);
void clearFlowBitFeild(void);
void enable(void);
void disable(void);
int isBuffer0Full(void);
int isBuffer1Full(void);
void resetBuffer0FullFlag(void);
void resetBuffer1FullFlag(void);
int whichBuffer(void);
uint32_t hzToPruTicks(float freq_hz);
float pruTicksToHz(uint32_t ticks);

void setKp(float Kp);
void setKd(float Kd);
void setAnklePos0(float pos0);
uint16_t getKp(void);
uint16_t getKd(void);
int16_t getAnklePos0(void);
int logFileInit(char* fileName);
void closeLogFile(void);

void clearBufferFlags(void);
void setTareEncoderBit(void);
#endif


