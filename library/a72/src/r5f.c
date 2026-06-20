/* Copyright 2026 Jonathan Realmuto -- Apache-2.0
 *
 * R5F runtime loader. The remoteproc sysfs dance mirrors pru.c: read state ->
 * stop -> write firmware name -> start -> wait running. The R5F graceful stop
 * needs firmware cooperation (park + control fw both implement the RP_MBOX_SHUTDOWN
 * handshake), so `echo stop` blocks until the core quiesces. main_r5fss0 runs in
 * LOCKSTEP, so there is ONE rproc (rp_r5f0); core1 (rp_r5f1) does not probe.
=============================================================================*/
#include "r5f.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static int r5f_stop(const char* node) {
  char path[256], buf[64];
  snprintf(path, sizeof(path), "%s/state", node);
  int fd = open(path, O_RDWR);
  if (fd == -1) { printf("R5F: open %s failed\n", path); return -1; }
  memset(buf, 0, sizeof(buf));
  int rtn = read(fd, buf, sizeof(buf) - 1);
  if (rtn > 0 && strcmp(buf, "offline\n") == 0) { close(fd); return 0; }
  rtn = write(fd, "stop", 4);          // blocks until the graceful-stop handshake completes
  close(fd);
  if (rtn == -1) { printf("R5F: stop %s failed\n", node); return -1; }
  return 0;
}

static int r5f_start(const char* node, const char* fwname) {
  char path[256];
  snprintf(path, sizeof(path), "%s/firmware", node);
  int fd = open(path, O_WRONLY);
  if (fd == -1) { printf("R5F: open %s failed\n", path); return -1; }
  int rtn = write(fd, fwname, strlen(fwname));
  close(fd);
  if (rtn == -1) { printf("R5F: set firmware %s failed\n", fwname); return -1; }
  snprintf(path, sizeof(path), "%s/state", node);
  fd = open(path, O_RDWR);
  if (fd == -1) { printf("R5F: open %s failed\n", path); return -1; }
  rtn = write(fd, "start", 5);
  close(fd);
  if (rtn == -1) { printf("R5F: start %s failed\n", node); return -1; }
  return 0;
}

static int r5f_wait_running(const char* node) {
  char path[256], buf[64];
  snprintf(path, sizeof(path), "%s/state", node);
  for (int i = 0; i < 100; i++) {        // ~1 s ceiling
    int fd = open(path, O_RDONLY);
    if (fd == -1) return -1;
    memset(buf, 0, sizeof(buf));
    int rtn = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    if (rtn > 0 && strcmp(buf, "running\n") == 0) return 0;
    usleep(10000);
  }
  return -1;
}

int R5fInit(char* suffix) {
  char fw[64];
  snprintf(fw, sizeof(fw), "j7-r5f0-%s-fw.ow", suffix);
  if (r5f_stop(rp_r5f0) == -1) return -1;
  if (r5f_start(rp_r5f0, fw) == -1) return -1;
  if (r5f_wait_running(rp_r5f0) == -1) { printf("R5F: not running\n"); return -1; }
  printf("R5F control firmware loaded (lockstep).\n");
  return 0;
}

int R5fCleanup(void) {
  r5f_stop(rp_r5f0);
  return 0;
}
