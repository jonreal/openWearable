/* Copyright 2026 Jonathan Realmuto

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

#include "nn.h"
#include "nnloop.h"
#include "fix16.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/mman.h>

extern volatile sig_atomic_t sigexit;   // set by the SIGINT handler (ui.c)

// --- A72<->C7x DDR mailbox -- MUST match tidl/src/ow_c7x_tidl.c -------------
#define MB_BASE     0xB8000000UL
#define MB_LEN      0x02200000UL        // ~34 MB window (covers NET/IO/INPUT/OUTPUT)
#define MB_READY    0x00000000          // A72->C7x kick   (RDY_MAGIC)
#define MB_DONE     0x00000010          // C7x->A72 ack    (DONE_MAGIC)
#define MB_STATUS   0x00000020          // TIDLRT return code
#define MB_NETLEN   0x00000030
#define MB_IOLEN    0x00000038
#define MB_NET      0x00100000          // compiled net.bin
#define MB_IO       0x02000000          // io descriptor
#define MB_INPUT    0x02100000          // float[N_FEAT]
#define MB_OUTPUT   0x02180000          // float[N_OUT]
#define RDY_MAGIC   0xC7C7AA01u
#define DONE_MAGIC  0xC7C7DD01u

// Free-running pace (~500 Hz). Bounds A72/C7x load; becomes a runtime param when
// inference cost matters (see notes/milestone-1-hetero/C7X-MLP-EXEC-PLAN.md).
#define NN_PERIOD_US   2000
#define NN_TIMEOUT_US  100000           // 100 ms ceiling per invoke

static int               g_fd  = -1;
static volatile uint8_t* g_mb  = NULL;
static pru_mem_t*        g_mem = NULL;
static pthread_t         g_thr;
static volatile int      g_run = 0;

static inline void     wr32(uint32_t off, uint32_t v) { *(volatile uint32_t*)(g_mb + off) = v; }
static inline uint32_t  rd32(uint32_t off) { return *(volatile uint32_t*)(g_mb + off); }

// Stage a file into the mailbox at `off`. Returns byte count, or -1 on error.
static long stage_file(const char* path, uint32_t off, uint32_t cap) {
  FILE* f = fopen(path, "rb");
  if (!f) { printf("NN: cannot open %s\n", path); return -1; }
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (n < 0 || (uint32_t)n > cap) {
    printf("NN: %s size %ld exceeds region %u\n", path, n, cap);
    fclose(f);
    return -1;
  }
  size_t got = fread((void*)(g_mb + off), 1, (size_t)n, f);
  fclose(f);
  if (got != (size_t)n) { printf("NN: short read on %s\n", path); return -1; }
  return n;
}

// One blocking C7x invoke. Returns the C7x STATUS (>= 0) or a negative host
// error (-2 = shutting down, -3 = timeout).
static int nn_invoke(const float* in, float* out) {
  wr32(MB_DONE, 0);
  memcpy((void*)(g_mb + MB_INPUT), in, N_FEAT * sizeof(float));
  __sync_synchronize();
  wr32(MB_READY, RDY_MAGIC);              // kick (firmware clears READY itself)

  struct timespec t0;
  clock_gettime(CLOCK_MONOTONIC, &t0);
  while (rd32(MB_DONE) != DONE_MAGIC) {
    if (!g_run || sigexit) return -2;
    struct timespec t1;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    long us = (t1.tv_sec - t0.tv_sec) * 1000000L + (t1.tv_nsec - t0.tv_nsec) / 1000L;
    if (us > NN_TIMEOUT_US) return -3;
  }
  int st = (int)rd32(MB_STATUS);
  memcpy(out, (void*)(g_mb + MB_OUTPUT), N_OUT * sizeof(float));
  return st;
}

static void* nn_thread(void* arg) {
  (void)arg;
  // Keep the control-side signals on the main thread (timer-driven logging,
  // stdin, Ctrl-C) -- this worker must not steal SIGALRM/SIGIO/SIGINT.
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGALRM);
  sigaddset(&set, SIGIO);
  sigaddset(&set, SIGINT);
  pthread_sigmask(SIG_BLOCK, &set, NULL);

  nn_feat_t feat;
  float out[N_OUT];

  while (g_run && !sigexit) {
    NnForward(g_mem, &feat);
    uint32_t stamp = g_mem->s->state[g_mem->s->cbuff_index].frame;

    int st = nn_invoke(feat.x, out);

    cpudata_t* cd = &g_mem->s->cpudata;
    if (st >= 0) {
      cd->nn.seq++;                       // odd: write in progress (seqlock)
      __sync_synchronize();
      for (int i = 0; i < N_OUT; i++)
        cd->nn.y[i] = fix16_from_float(out[i]);
      cd->nn.stamp  = stamp;
      cd->nn.status = st;
      __sync_synchronize();
      cd->nn.seq++;                       // even: result stable
      NnInterpret(&cd->nn);
    } else {
      cd->nn.status = st;                 // host error -- hold last good y[]
      if (st == -2) break;                // shutting down
    }
    usleep(NN_PERIOD_US);
  }
  return NULL;
}

int NnStart(pru_mem_t* pru_mem, const char* net_path, const char* io_path) {
  g_mem = pru_mem;

  g_fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (g_fd < 0) { printf("NN: open(/dev/mem) failed\n"); return -1; }

  g_mb = (volatile uint8_t*)mmap(NULL, MB_LEN, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, g_fd, MB_BASE);
  if (g_mb == MAP_FAILED) {
    printf("NN: mmap(0x%lx) failed\n", MB_BASE);
    close(g_fd); g_fd = -1; g_mb = NULL;
    return -1;
  }

  long nlen = stage_file(net_path, MB_NET, MB_IO    - MB_NET);
  long ilen = stage_file(io_path,  MB_IO,  MB_INPUT - MB_IO);
  if (nlen < 0 || ilen < 0) { NnStop(); return -1; }
  wr32(MB_NETLEN, (uint32_t)nlen);
  wr32(MB_IOLEN,  (uint32_t)ilen);
  wr32(MB_DONE, 0);
  printf("NN: staged net=%ldB io=%ldB; starting inference thread.\n", nlen, ilen);

  NnInit();

  g_run = 1;
  if (pthread_create(&g_thr, NULL, nn_thread, NULL) != 0) {
    printf("NN: pthread_create failed\n");
    g_run = 0;
    NnCleanup();
    NnStop();
    return -1;
  }
  return 0;
}

void NnStop(void) {
  if (g_run) {
    g_run = 0;
    pthread_join(g_thr, NULL);
    NnCleanup();
  }
  if (g_mb && g_mb != MAP_FAILED) { munmap((void*)g_mb, MB_LEN); g_mb = NULL; }
  if (g_fd >= 0) { close(g_fd); g_fd = -1; }
}
