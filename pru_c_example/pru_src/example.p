// PRU

.origin 0
.entrypoint START

#define CONST_PRUCFG         C4
#define CONST_PRUSHAREDRAM   C28

#define PRU0_CTRL            0x22000
#define PRU1_CTRL            0x24000

#define CTPPR0               0x28

#define OWN_RAM              0x000
#define OTHER_RAM            0x020
#define SHARED_RAM           0x100

#define PRU0_R31_VEC_VALID (1<<5)
#define SIGNUM 3

#define NUM_OF_LOOPS 3
#define DELAY_SEC 1
#define CLOCK 200000000
#define CLOCKS_PER_LOOP 10
#define DELAYCOUNT DELAY_SEC * CLOCK / CLOCKS_PER_LOOP

START:
  mov     r0, 0xDEADBEAF
  sbco    r0, CONST_PRUSHAREDRAM, 0, 4
  mov     r0, 0xAFAFAFAF
  sbco    r0, CONST_PRUSHAREDRAM, 4, 4

END:
  mov     r31.b0, PRU0_R31_VEC_VALID | SIGNUM
  HALT




