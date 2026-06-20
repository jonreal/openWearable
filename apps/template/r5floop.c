/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * Template R5F hooks. main_r5fss0 runs in LOCKSTEP, so there is one logical R5F core
 * (core0, the lockstep primary). Edit the hooks; the framework (r5f0_main.c) owns the
 * frame-count decimation and the rproc stop-handshake -- exactly like pruloop.c.
=============================================================================*/
#include "r5floop.h"

void R5f0Init(pru_mem_t* mem) {
}
void R5f0UpdateState(const r5f_view_t* view, r5f_io_t* io) {
  io->s->r5fvar++;
}
void R5f0UpdateControl(const r5f_view_t* view, r5f_io_t* io) {
}
void R5f0Cleanup(void) {
}
