#ifndef _VICONSYNC_H_
#define _VICONSYNC_H_

#define SYNC_PIN  (16)

/* Digital input reg. ------------------------------------------------------ */
volatile register uint32_t __R31;

/* Global ------------------------------------------------------------------ */ 
extern volatile uint32_t *debugBuffer;

/* Prototypes -------------------------------------------------------------- */
uint16_t viconSync(void);

#endif


