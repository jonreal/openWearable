/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
=============================================================================*/
#include "r5floop.h"

// ---------------------------------------------------------------------------
// R5F core0 -- edit the hooks; the framework (r5f0_main.c) owns the frame-count
// decimation and the rproc stop-handshake.
// ---------------------------------------------------------------------------
void R5f0Init(pru_mem_t* mem) {
}
void R5f0UpdateState(const r5f_view_t* view, r5f_io_t* io) {
  io->s->r5fvar++;
}
void R5f0UpdateControl(const r5f_view_t* view, r5f_io_t* io) {
}
void R5f0Cleanup(void) {
}

// ---------------------------------------------------------------------------
// R5F core1
// ---------------------------------------------------------------------------
void R5f1Init(pru_mem_t* mem) {
}
void R5f1UpdateState(const r5f_view_t* view, r5f_io_t* io) {
  io->s->r5fvar++;
}
void R5f1UpdateControl(const r5f_view_t* view, r5f_io_t* io) {
}
void R5f1Cleanup(void) {
}
