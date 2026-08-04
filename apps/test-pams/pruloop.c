/* test-pams -- single PAM muscle on PRU1: I2C pressure sampling (I2C2) + valve
 * control (PRG0_PRU1 GPO). Exercises the ported hardware-I2C driver and the four
 * valve DO pins. PRU0 is idle. Pd is set live from the UI (param->Pd).
=============================================================================*/
#include "pruloop.h"
#include "pam.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// ---- hardware config (edit to match your rig) ------------------------------
#define I2C_CH        2      // I2C2 @ 0x02020000 -> header P9.19 (SCL) / P9.20 (SDA)
#define MUX_ADDR      0x70   // PCA9548 I2C address
#define SENSOR_ADDR   0x28   // HSC/SSC pressure sensor I2C address
#define RES_CHAN      7      // mux channel: reservoir sensor
#define MUSCLE_CHAN   1      //
#define HP_PIN        2      // inflate valve -> R30.2 -> (P8.43)
#define LP_PIN        3      // deflate valve -> R30.3 -> (P8.44)
#define T_REFRACT     150    // refractory ticks after reaching target

// Spare valve DO already pinmuxed for future use: R30.4 (GPO4/P8.41), R30.5 (GPO5/P8.42).

i2c_t*       i2c;
i2cmux_t*    mux;
reservoir_t* reservoir;
pam_t*       pam;

// ---------------------------------------------------------------------------
// PRU0 -- idle (PRU1 owns the PAM)
// ---------------------------------------------------------------------------
void Pru0Init(pru_mem_t* mem) {}
void Pru0UpdateState(const pru_view_t* view, pru_io_t* io) {}
void Pru0UpdateControl(const pru_view_t* view, pru_io_t* io) {}
void Pru0Cleanup(void) {}

// ---------------------------------------------------------------------------
// PRU1 -- PAM: I2C pressure + valve control
// ---------------------------------------------------------------------------
void Pru1Init(pru_mem_t* mem) {

  i2c = I2cInit(I2C_CH);
  mux = MuxI2cInit(i2c, MUX_ADDR, PCA9548);

  reservoir = PamReservoirInit(PressureSensorInit(mux, RES_CHAN, SENSOR_ADDR));

  //pam = PamInitMuscle(PressureSensorInit(mux, MUSCLE_CHAN, SENSOR_ADDR),
  //                    reservoir, HP_PIN, LP_PIN, 0, 0, T_REFRACT, 0);
  //PamSetPd(pam, 0);
}

void Pru1UpdateState(const pru_view_t* view, pru_io_t* io) {
  //PamSetPd(pam, view->p->Pd);                          // UI-commanded target

  PamReservoirUpdate(reservoir);
  io->s->p_res = PamReservoirGetPressure(reservoir);

  //PamActionSimple(pam);                                // bang-bang -> valve command u
  //PamUpdate(pam);                                      // sample muscle pressure
  //io->s->pam_state = PamGetState(pam);
}

void Pru1UpdateControl(const pru_view_t* view, pru_io_t* io) {}

void Pru1Cleanup(void) {
  //PamMuscleFree(pam);
  //PamReservoirFree(reservoir);
}
