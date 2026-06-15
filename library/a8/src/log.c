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
#include <errno.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include "pru.h"
#include "format.h"
#include "dma.h"

// The DMA backend (AM335x EDMA3) lives in dma.c, behind the dma.h interface,
// so this logger stays board-agnostic.

// ---------------------------------------------------------------------------
// Function: void circBuffUpdate(void)
//
//  This function initializes the circular buffer.
// ---------------------------------------------------------------------------
void LogCircBuffUpdate(log_t* log) {
  if (log->cbuff->end != log->pru_mem->s->cbuff_index)
    log->cbuff->end = log->pru_mem->s->cbuff_index;
}

// ---------------------------------------------------------------------------
//
// Function Definitions
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Function: void circBuffInit(void)
//
//  This function initializes a circular buffer.
// ---------------------------------------------------------------------------
circbuff_t* LogNewCircBuff(void) {
  circbuff_t* cb = malloc(sizeof(circbuff_t));
  cb->start = 0;
  cb->end = 0;
  memset(cb->temp_buff,0, TEMP_BUFF_LEN);
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
//  Functions: int LogInit(const pru_mem_t pru_mem)
//
//  This function creates a log file.
//
//  TODO: check if file exists.
//  ------------------------------------------------------------------------- */
log_t* LogInit(const pru_mem_t* pru_mem) {
  // Create new datalog struct
  log_t* log = malloc(sizeof(log_t));
  log->pru_mem = pru_mem;
  log->fd = 0;
  log->location = 0;
  log->addr = NULL;
  log->cbuff = LogNewCircBuff();
  memset(log->write_buff, 0, WRITE_BUFF_LEN);
  
  // Initialize DMA backend (dma.c); falls back to memcpy if unavailable.
  log->dma_ctx = DmaInit(WRITE_BUFF_LEN);
  log->use_dma = DmaInitialized(log->dma_ctx);
  
  // Initialize stats flag
  log->show_stats = 0;  // Don't show stats by default
  
  if (log->use_dma) {
    printf("DMA logging enabled\n");
  } else {
    printf("DMA initialization failed or not available, using standard memcpy\n");
  }
  
  return log;
}


int LogNewFile(log_t* log, char* file) {

  // Open file, stretch and write blank
  log->fd = open(file, O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (lseek(log->fd, LOGSIZE, SEEK_SET) == -1){
    close(log->fd);
    printf("Error stretching file.\n");
    return -1;
  }
  if (write(log->fd, "", 1) == -1){
    close(log->fd);
    printf("Error writing blank at end of file.\n");
    return -1;
  }

  // memory map file
  log->addr = mmap(0, LOGSIZE,
                   PROT_WRITE,
                   MAP_SHARED | MAP_POPULATE,
                   log->fd, 0);
  if (log->addr == MAP_FAILED) {
    printf("mmap failed");
    close(log->fd);
    return -1;
  }

  mlock(log->addr, LOGSIZE);

  // ---- ASCII header, terminated by a "#DATA" line --------------------------
  // Everything up to and including "#DATA\n" is human-readable metadata; a
  // host-side decoder parses #fields/#record_bytes here, then reads the raw
  // fixed-size binary records that follow. Each chunk is built in write_buff
  // and memcpy'd to the mmap'd file, advancing log->location.
  int len = 0;
  char time_str[1024];

  // magic / version
  len = sprintf(log->write_buff, "#openWearable-log v1\n");
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;

  // date + sample rate
  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  strftime(time_str, sizeof(time_str), "%d-%b-%Y %X", t);
  len = sprintf(log->write_buff, "#date: %s\n#fs_hz: %u\n",
                time_str, log->pru_mem->p->fs_hz);
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;

  // memory allocation (informational comment lines)
  log->write_buff[0] = '\0';
  PruSprintMalloc(log->pru_mem, log->write_buff);
  len = strlen(log->write_buff);
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;

  // schema: #fields / #record_bytes (per-app, from format.c)
  log->write_buff[0] = '\0';
  FormatLogSchema(log->write_buff);
  len = strlen(log->write_buff);
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;

  // start-of-binary-data sentinel
  len = sprintf(log->write_buff, "#DATA\n");
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;

  log->write_buff[0] = '\0';
  return 0;
}

void LogWriteStateToFile(log_t* log) {
  static struct timespec last_time;
  static struct timespec curr_time;
  static struct timespec format_time_start, format_time_end;
  static struct timespec write_time_start, write_time_end;
  static uint64_t total_bytes = 0;
  static int first_call = 1;
  static double total_seconds = 0.0;
  static int dma_transfers = 0;
  static int dma_errors = 0;
  static double format_time_ms_total = 0.0;
  static double write_time_ms_total = 0.0;
  static int report_count = 0;
  
  // Initialize timer on first call
  if (first_call) {
    clock_gettime(CLOCK_MONOTONIC, &last_time);
    first_call = 0;
  }
  
  log->write_buff[0] = '\0';
  int size =
    ((STATE_BUFF_LEN + log->cbuff->end - log->cbuff->start) % STATE_BUFF_LEN);

  if (size > MIN_STATE_REQ) {
    // Pack the available states as fixed-size binary records straight into
    // write_buff (pointer-advance, no strcat). Never overflow the buffer — any
    // remainder is picked up on the next flush.
    int recbytes = FormatLogRecordBytes();
    if (size * recbytes > WRITE_BUFF_LEN)
      size = WRITE_BUFF_LEN / recbytes;

    // Measure packing time
    clock_gettime(CLOCK_MONOTONIC, &format_time_start);

    uint8_t* wp = (uint8_t*) log->write_buff;
    for(int i=log->cbuff->start; i<log->cbuff->start+size; i++){
      FormatLogRecord(&log->pru_mem->s->state[i % STATE_BUFF_LEN], wp);
      wp += recbytes;
    }

    clock_gettime(CLOCK_MONOTONIC, &format_time_end);
    double format_ms = ((format_time_end.tv_sec - format_time_start.tv_sec) * 1000.0) + 
                      ((format_time_end.tv_nsec - format_time_start.tv_nsec) / 1000000.0);
    format_time_ms_total += format_ms;

    log->cbuff->start = (log->cbuff->start + size) % STATE_BUFF_LEN;

    // Write to file (raw binary record block)
    int len = size * recbytes;

    // Measure write time
    clock_gettime(CLOCK_MONOTONIC, &write_time_start);
    
    // Always use memcpy for the first few transfers to establish baseline
    static int transfer_count = 0;
    
    if (transfer_count < 100) {
        // Start with memcpy for the first 100 transfers
        memcpy(log->addr + log->location, log->write_buff, len);
        transfer_count++;
    } else if (log->use_dma && DmaInitialized(log->dma_ctx)) {
        // After establishing baseline, try DMA
        dma_transfers++;
        if (DmaTransfer(log->dma_ctx, log->write_buff, log->addr + log->location, len) < 0) {
            // Fall back to memcpy if DMA transfer fails
            memcpy(log->addr + log->location, log->write_buff, len);
            dma_errors++;
            
            // If we get too many consecutive errors, disable DMA
            if (dma_errors > 20 && dma_errors == dma_transfers) {
                printf("Too many DMA errors, disabling DMA\n");
                log->use_dma = 0;
            }
        }
    } else {
        memcpy(log->addr + log->location, log->write_buff, len);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &write_time_end);
    double write_ms = ((write_time_end.tv_sec - write_time_start.tv_sec) * 1000.0) + 
                     ((write_time_end.tv_nsec - write_time_start.tv_nsec) / 1000000.0);
    write_time_ms_total += write_ms;
    
    log->location += len;
    
    // Update statistics
    total_bytes += len;
    report_count++;
    
    // Calculate throughput (every ~1 second to avoid flooding terminal)
    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    double elapsed = (curr_time.tv_sec - last_time.tv_sec) + 
                    (curr_time.tv_nsec - last_time.tv_nsec) / 1.0e9;
    
    total_seconds += elapsed;
    
    if (elapsed >= 1.0) {
      double current_kbps = (len * 8.0) / (elapsed * 1024.0);
      double avg_kbps = (total_bytes * 8.0) / (total_seconds * 1024.0);
      
      // Calculate average operation times
      double avg_format_ms = format_time_ms_total / (double)report_count;
      double avg_write_ms = write_time_ms_total / (double)report_count;
      
      // Print throughput statistics if show_stats is enabled
      if (log->show_stats) {
        printf("[LOG] Throughput: %.2f kbps current | %.2f kbps avg | %.2f MB total", 
               current_kbps, avg_kbps, total_bytes / (1024.0 * 1024.0));
        
        // Print DMA statistics if enabled
        if (log->use_dma) {
          printf(" | DMA: %d transfers, %d errors", dma_transfers, dma_errors);
        }
        
        // Print operation timing
        printf(" | Format: %.3f ms, Write: %.3f ms", avg_format_ms, avg_write_ms);
        printf("\n");
      }
      
      // Reset timer and counters
      last_time = curr_time;
      format_time_ms_total = 0.0;
      write_time_ms_total = 0.0;
      report_count = 0;
    }
  }
}

int LogSaveFile(log_t* log) {

  munlock(log->addr, LOGSIZE);
  // Unmap and truncate file
  if (munmap(log->addr, LOGSIZE) == -1){
    close(log->fd);
    printf("unmap failed\n");
    return -1;
  }
  if (ftruncate(log->fd, log->location) == -1){
    close(log->fd);
    printf("Error truncating file.\n");
    return -1;
  }
  close(log->fd);
  log->fd = 0;
  log->location = 0;
  log->addr = NULL;
  memset(log->cbuff->temp_buff,0, TEMP_BUFF_LEN);
  memset(log->write_buff, 0, WRITE_BUFF_LEN);
  return 0;
}

void LogCleanup(log_t* log) {
  // Release the DMA backend (NULL-safe; clean up even if use_dma was disabled
  // at runtime after transfer errors).
  if (log->dma_ctx) {
    DmaCleanup(log->dma_ctx);
    log->dma_ctx = NULL;
  }

  free(log);
}


