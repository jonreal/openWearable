/* Copyright 2018 Jonathan Realmuto

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

#include "log.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include "pru.h"
#include "format.h"

// Logging is split across two contexts:
//   producer - the real-time SIGALRM timer callback (UiTimerCallback). It must
//              never block, so it only packs records and pushes them into a RAM
//              ring (lock-free, async-signal-safe ops only).
//   consumer - a writer thread that drains the ring to the file with write().
// This keeps disk/SD latency off the control loop and makes run length
// unbounded (no preallocated mmap, no mlock of the whole file).

// ---------------------------------------------------------------------------
//  SPSC ring buffer (single producer / single consumer, lock-free)
// ---------------------------------------------------------------------------

// Producer: copy 'len' bytes in. Returns 0 on success, -1 if full (caller drops).
static int RingPush(ringbuf_t* r, const void* data, size_t len) {
  size_t head = r->head;                                   // producer owns head
  size_t tail = __atomic_load_n(&r->tail, __ATOMIC_ACQUIRE);
  if (r->cap - (head - tail) < len)
    return -1;                                             // not enough room
  size_t off = head & (r->cap - 1);
  size_t first = (len < r->cap - off) ? len : (r->cap - off);
  memcpy(r->buf + off, data, first);
  if (len > first)
    memcpy(r->buf, (const uint8_t*) data + first, len - first);  // wrap
  __atomic_store_n(&r->head, head + len, __ATOMIC_RELEASE);
  return 0;
}

// Consumer: drain everything currently available to the file descriptor.
static void RingDrain(ringbuf_t* r, int fd) {
  for (;;) {
    size_t tail = r->tail;                                 // consumer owns tail
    size_t head = __atomic_load_n(&r->head, __ATOMIC_ACQUIRE);
    size_t avail = head - tail;
    if (avail == 0)
      break;
    size_t off = tail & (r->cap - 1);
    size_t chunk = (avail < r->cap - off) ? avail : (r->cap - off);  // contiguous
    ssize_t w = write(fd, r->buf + off, chunk);
    if (w <= 0)
      break;                                               // write error; retry next wake
    __atomic_store_n(&r->tail, tail + (size_t) w, __ATOMIC_RELEASE);
  }
}

// Writer thread: wait for data, drain it, repeat until shutdown.
static void* LogWriter(void* arg) {
  log_t* log = (log_t*) arg;
  while (1) {
    sem_wait(&log->wake);
    RingDrain(&log->ring, log->fd);
    if (!log->running) {
      RingDrain(&log->ring, log->fd);                      // final flush
      break;
    }
  }
  return NULL;
}

// ---------------------------------------------------------------------------
// Function: void LogCircBuffUpdate(log_t* log)
//
//  Mirror the PRU state-ring write index into our local circular buffer.
// ---------------------------------------------------------------------------
void LogCircBuffUpdate(log_t* log) {
  if (log->cbuff->end != log->pru_mem->s->cbuff_index)
    log->cbuff->end = log->pru_mem->s->cbuff_index;
}

// ---------------------------------------------------------------------------
// Function: circbuff_t* LogNewCircBuff(void)
//
//  Allocate and initialize a circular buffer.
// ---------------------------------------------------------------------------
circbuff_t* LogNewCircBuff(void) {
  circbuff_t* cb = malloc(sizeof(circbuff_t));
  cb->start = 0;
  cb->end = 0;
  memset(cb->temp_buff, 0, TEMP_BUFF_LEN);
  return cb;
}

void LogDebugWriteState(const shared_mem_t* sm, circbuff_t* cb, char* buff) {
    if (cb->start != sm->state[0].time){
      FormatSprintState(&sm->state[0], buff);
      fprintf(stdout, buff);
      buff[0] = '\0';
      cb->start = sm->state[0].time;
    }
}

// ----------------------------------------------------------------------------
//  Function: log_t* LogInit(const pru_mem_t* pru_mem)
//
//  Allocate the logger and its RAM ring. The writer thread is started per file
//  (LogNewFile), not here.
//  ------------------------------------------------------------------------- */
log_t* LogInit(const pru_mem_t* pru_mem) {
  log_t* log = malloc(sizeof(log_t));
  log->pru_mem = pru_mem;
  log->fd = -1;
  log->cbuff = LogNewCircBuff();
  memset(log->write_buff, 0, WRITE_BUFF_LEN);

  log->ring.buf = malloc(LOG_RING_CAP);
  log->ring.cap = LOG_RING_CAP;
  log->ring.head = 0;
  log->ring.tail = 0;
  if (log->ring.buf) {
    memset(log->ring.buf, 0, LOG_RING_CAP);  // pre-fault the pages (resident)
    mlock(log->ring.buf, LOG_RING_CAP);      // pin the (small) ring, not the file
  }

  log->running = 0;
  log->dropped = 0;
  log->total_bytes = 0;
  log->show_stats = 0;
  sem_init(&log->wake, 0, 0);

  return log;
}

int LogNewFile(log_t* log, char* file) {
  log->fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (log->fd < 0) {
    printf("Error opening log file %s\n", file);
    return -1;
  }

  // ASCII header (written directly, before the writer thread starts): magic,
  // metadata, schema, then a "#DATA" sentinel. Raw binary records follow.
  char hdr[4096];
  char tmp[1024];
  int len = 0;
  len += sprintf(hdr + len, "#openWearable-log v1\n");

  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  char time_str[256];
  strftime(time_str, sizeof(time_str), "%d-%b-%Y %X", t);
  len += sprintf(hdr + len, "#date: %s\n#fs_hz: %u\n", time_str, log->pru_mem->p->fs_hz);

  tmp[0] = '\0';
  PruSprintMalloc(log->pru_mem, tmp);          // informational #comment lines
  len += sprintf(hdr + len, "%s", tmp);

  tmp[0] = '\0';
  FormatLogSchema(tmp);                         // #fields / #record_bytes
  len += sprintf(hdr + len, "%s", tmp);

  len += sprintf(hdr + len, "#DATA\n");
  if (write(log->fd, hdr, len) != len)
    printf("Warning: short header write\n");

  // Reset ring and start the writer thread.
  log->ring.head = 0;
  log->ring.tail = 0;
  log->dropped = 0;
  log->total_bytes = 0;
  log->running = 1;
  if (pthread_create(&log->writer, NULL, LogWriter, log) != 0) {
    printf("Error starting log writer thread\n");
    log->running = 0;
    close(log->fd);
    log->fd = -1;
    return -1;
  }
  return 0;
}

// Producer side - runs in the real-time timer callback. Packs the available
// PRU states into write_buff and pushes the block into the ring (non-blocking).
void LogWriteStateToFile(log_t* log) {
  static struct timespec t0, tlast;
  static int started = 0;

  if (!started) {
    clock_gettime(CLOCK_MONOTONIC, &t0);
    tlast = t0;
    started = 1;
  }

  int size =
    ((STATE_BUFF_LEN + log->cbuff->end - log->cbuff->start) % STATE_BUFF_LEN);

  if (size > MIN_STATE_REQ) {
    int recbytes = FormatLogRecordBytes();
    if ((size_t)(size * recbytes) > WRITE_BUFF_LEN)
      size = WRITE_BUFF_LEN / recbytes;           // never overflow staging buf

    uint8_t* wp = (uint8_t*) log->write_buff;
    for (int i = log->cbuff->start; i < log->cbuff->start + size; i++) {
      FormatLogRecord(&log->pru_mem->s->state[i % STATE_BUFF_LEN], wp);
      wp += recbytes;
    }
    log->cbuff->start = (log->cbuff->start + size) % STATE_BUFF_LEN;

    size_t len = (size_t)(size * recbytes);
    if (RingPush(&log->ring, log->write_buff, len) == 0) {
      log->total_bytes += len;
      sem_post(&log->wake);                       // async-signal-safe wake
    } else {
      log->dropped += size;                       // ring full: disk too slow
    }

    // Periodic throughput/drop report (-s). printf in a signal handler isn't
    // strictly async-signal-safe, but it is diagnostic-only, as before.
    if (log->show_stats) {
      struct timespec now;
      clock_gettime(CLOCK_MONOTONIC, &now);
      double since = (now.tv_sec - tlast.tv_sec) + (now.tv_nsec - tlast.tv_nsec) / 1e9;
      if (since >= 1.0) {
        double total = (now.tv_sec - t0.tv_sec) + (now.tv_nsec - t0.tv_nsec) / 1e9;
        double avg_kbps = (log->total_bytes * 8.0) / (total * 1024.0);
        size_t ring_used = log->ring.head - log->ring.tail;
        printf("[LOG] %.2f kbps avg | %.2f MB | dropped %llu rec | ring %zu/%zu KB\n",
               avg_kbps, log->total_bytes / (1024.0 * 1024.0),
               (unsigned long long) log->dropped,
               ring_used / 1024, log->ring.cap / 1024);
        tlast = now;
      }
    }
  }
}

// Stop logging: tell the writer to drain + exit, then close the file.
// By the time this is called the producer (timer callback) has stopped pushing
// (UiStopAndSaveLog clears the logging flag first), so no records race in.
int LogSaveFile(log_t* log) {
  if (log->fd < 0)
    return 0;

  log->running = 0;
  sem_post(&log->wake);                  // wake writer to do its final drain
  pthread_join(log->writer, NULL);

  fsync(log->fd);
  close(log->fd);
  log->fd = -1;

  if (log->dropped)
    printf("Log closed: %llu records dropped (disk could not keep up)\n",
           (unsigned long long) log->dropped);
  return 0;
}

void LogCleanup(log_t* log) {
  if (!log)
    return;
  if (log->fd >= 0)
    LogSaveFile(log);                    // stop thread + close if still open

  if (log->ring.buf) {
    munlock(log->ring.buf, log->ring.cap);
    free(log->ring.buf);
  }
  sem_destroy(&log->wake);
  free(log->cbuff);
  free(log);
}
