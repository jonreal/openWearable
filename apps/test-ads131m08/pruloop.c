/* test-ads131m08 -- the R5F drives the ADS131M08 over McSPI7 (see r5floop.c).
 * The PRU does no sensing here; it only runs the control-loop timer so the R5F
 * loop is clocked (r5f0_main decimates off the PRU frame counter).
=============================================================================*/
#include "pruloop.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// --- PRU0 -------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {}
void Pru0UpdateState(const pru_view_t* view, pru_io_t* io) {}
void Pru0UpdateControl(const pru_view_t* view, pru_io_t* io) {}
void Pru0Cleanup(void) {}

// --- PRU1 -------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {}
void Pru1UpdateState(const pru_view_t* view, pru_io_t* io) {}
void Pru1UpdateControl(const pru_view_t* view, pru_io_t* io) {}
void Pru1Cleanup(void) {}
