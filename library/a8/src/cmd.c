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

#include "cmd.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define CMD_PORT 1501

static pru_mem_t*    g_pm    = NULL;
static int           g_sock  = -1;
static pthread_t     g_thread;
static volatile int  g_run   = 0;

// Per-app hook: weak no-op so apps that don't accept commands still link. An app
// overrides it (see apps/<app>/uiloop.c) to map names -> its params / cmd bits.
__attribute__((weak)) void CmdApply(pru_mem_t* pm, const char* name, float value) {
  (void) pm; (void) name; (void) value;
}

static void* CmdRxLoop(void* arg) {
  (void) arg;
  char buf[256];
  while (g_run) {
    ssize_t n = recv(g_sock, buf, sizeof(buf) - 1, 0);
    if (n <= 0) continue;                 // timeout / error -> re-check g_run
    buf[n] = '\0';
    char name[64];
    float value;
    if (sscanf(buf, "%63s %f", name, &value) != 2)
      continue;
    if (strcmp(name, "arm") == 0) {       // the single gate (A8 owns the arm word)
      if (value != 0.0f) g_pm->s->arm |=  ARM_ARMED;
      else               g_pm->s->arm &= ~ARM_ARMED;
    } else if (g_pm->s->arm & ARM_ARMED) {   // no other change unless armed
      CmdApply(g_pm, name, value);
    }
  }
  return NULL;
}

void CmdInit(pru_mem_t* pm) {
  g_pm = pm;
  g_pm->s->arm &= ~ARM_ARMED;             // disarmed at startup

  g_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (g_sock < 0) { printf("cmd: socket failed\n"); return; }

  struct sockaddr_in a;
  memset(&a, 0, sizeof(a));
  a.sin_family = AF_INET;
  a.sin_addr.s_addr = htonl(INADDR_ANY);
  a.sin_port = htons(CMD_PORT);
  if (bind(g_sock, (struct sockaddr*) &a, sizeof(a)) < 0) {
    printf("cmd: cannot bind port %d\n", CMD_PORT);
    close(g_sock); g_sock = -1; return;
  }
  // recv timeout so the thread can notice g_run==0 and exit.
  struct timeval tv = { 0, 200000 };
  setsockopt(g_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  g_run = 1;
  pthread_create(&g_thread, NULL, CmdRxLoop, NULL);
  printf("cmd channel: listening on udp:%d (disarmed)\n", CMD_PORT);
}

void CmdCleanup(void) {
  if (!g_run) return;
  g_run = 0;
  pthread_join(g_thread, NULL);
  if (g_sock >= 0) close(g_sock);
  g_sock = -1;
}
