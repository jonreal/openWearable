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

#ifndef _LOG_
#define _LOG_

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <semaphore.h>
#include "mem_types.h"

#define TEMP_BUFF_LEN   1024
#define WRITE_BUFF_LEN  65536                  // producer staging buffer
#define MIN_STATE_REQ   2
#define LOG_RING_CAP    (4u * 1024u * 1024u)   // RAM ring, power of two (~4 MB)

// Circular Buffer Struct (mirrors the PRU state-ring index)
typedef struct{
  uint32_t start;
  uint32_t end;
  char temp_buff[TEMP_BUFF_LEN];
} circbuff_t;

// SPSC byte ring: producer = real-time timer callback, consumer = writer
// thread. head/tail are monotonic byte counters (never wrapped); the buffer
// index is pos & (cap-1), so cap must be a power of two.
typedef struct {
  uint8_t* buf;
  size_t   cap;
  volatile size_t head;     // bytes produced (producer-owned)
  volatile size_t tail;     // bytes consumed (consumer-owned)
} ringbuf_t;

// Log File Struct
typedef struct{
  int fd;
  circbuff_t* cbuff;
  char write_buff[WRITE_BUFF_LEN];   // producer packs records here, then pushes
  const pru_mem_t* pru_mem;

  // Disk is decoupled from the real-time path: the timer callback only packs
  // records into 'ring'; a writer thread drains the ring to the file with
  // write(). If disk can't keep up the ring fills and records are dropped
  // (counted) rather than stalling the control loop. Unbounded run length.
  ringbuf_t ring;
  pthread_t writer;
  sem_t     wake;                    // producer posts; writer waits
  volatile int running;             // writer-thread run flag
  volatile uint64_t dropped;        // records dropped (ring full = disk too slow)
  volatile uint64_t total_bytes;    // record bytes handed to the ring

  int show_stats;                    // -s: print throughput/drops
} log_t;

circbuff_t* LogNewCircBuff(void);
void LogCircBuffUpdate(log_t* log);
void LogDebugWriteState(const shared_mem_t* sm, circbuff_t* cb, char* buff);
log_t* LogInit(const pru_mem_t* pru_mem);
int LogNewFile(log_t* log, char* file);
void LogWriteStateToFile(log_t* log);
int LogSaveFile(log_t* log);
void LogCleanup(log_t* log);

#endif /* _LOG_ */
