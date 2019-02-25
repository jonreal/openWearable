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
#include "pru.h"

// ---------------------------------------------------------------------------
// Function: void circBuffInit(void)
//
//  This function initializes a circular buffer.
// ---------------------------------------------------------------------------
circbuff_t* LogNewCircBuff(void) {
  circbuff_t* cb = malloc(sizeof(circbuff_t));
  cb->start = 0;
  cb->end = 0;
  for (int i=0; i<TEMP_BUFF_LEN; i++)
    cb->temp_buff[0] = 0;

  return cb;
}

// ---------------------------------------------------------------------------
// Function: void circBuffUpdate(void)
//
//  This function initializes the circular buffer.
// ---------------------------------------------------------------------------
void LogCircBuffUpdate(const volatile uint32_t new_cb_index, circbuff_t* cb) {
  if (cb->end != new_cb_index)
    cb->end = new_cb_index;
}

void LogDebugWriteState(const shared_mem_t* sm, circbuff_t* cb, char* buff) {
    if (cb->start != sm->state[0].time){
      PruSprintState(&sm->state[0], buff);
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
  memset(&log->write_buff[0], 0, sizeof(log->write_buff));
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

  // Log time
  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  int len = 0;
  char time_str[1024];

  memcpy(log->write_buff, "#Date: ", 7);
  strftime(time_str, 1024, "%d-%b-%Y %X\n", t);
  strcat(log->write_buff, time_str);
  len = strlen(log->write_buff);
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;
  log->write_buff[0] = '\0';

  // Log memory allocation
  PruSprintMalloc(log->pru_mem, log->write_buff);
  len = strlen(log->write_buff);
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;
  log->write_buff[0] = '\0';

  // Log parameters
  PruSprintParams(log->pru_mem->p, log->write_buff);
  len = strlen(log->write_buff);
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;
  log->write_buff[0] = '\0';

  // Log header
  PruSprintStateHeader(log->write_buff);
  len = strlen(log->write_buff);
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;
  log->write_buff[0] = '\0';
  return 0;
}

void LogWriteStateToFile(log_t* log) {
  LogCircBuffUpdate(log->pru_mem->s->cbuff_index, log->cbuff);
  log->write_buff[0] = '\0';
  int size =
    ((STATE_BUFF_LEN + log->cbuff->end - log->cbuff->start) % STATE_BUFF_LEN);

  if (size >= MIN_STATE_REQ) {
    for(int i=log->cbuff->start; i<log->cbuff->start+size; i++){
      memset(log->cbuff->temp_buff, 0, TEMP_BUFF_LEN);
      PruSprintState(&log->pru_mem->s->state[i % STATE_BUFF_LEN],
                     log->cbuff->temp_buff);
      strcat(log->write_buff, log->cbuff->temp_buff);
    }

    log->cbuff->start = (log->cbuff->start + size) % STATE_BUFF_LEN;

    // Write to file
    int len = strlen(log->write_buff);
    memcpy(log->addr + log->location, log->write_buff, len);
    log->location += len;
  }
}


void LogWriteStateToFileAndPublish(int logflag,
                                   log_t* log,
                                   udp_t* udp) {
  LogCircBuffUpdate(log->pru_mem->s->cbuff_index, log->cbuff);
  log->write_buff[0] = '\0';
  int size =
    ((STATE_BUFF_LEN + log->cbuff->end - log->cbuff->start) % STATE_BUFF_LEN);

  // send samples at approx 30 Hz (if MIN_STATE_REQ = 33 and fs = 1000)
  if (size >= MIN_STATE_REQ) {
    udp->buff[0] = '\0';
    int i = log->cbuff->start + (size -1);
    sprintf(udp->buff,
            "%u\t"        // timeStamp - uint32_t
            "%f\t"        // angle - fix16_t
            "%f\t"        // angle_d - fix16_t
            "\n",
            log->pru_mem->s->state[i % STATE_BUFF_LEN].time,
            fix16_to_float(log->pru_mem->s->state[i % STATE_BUFF_LEN].angle),
            fix16_to_float(log->pru_mem->s->state[i % STATE_BUFF_LEN].angle_d)
            );
    UdpTxPacket(udp);

//    printf("%i\t%u\t%f\t%f\n", size, 
//          log->pru_mem->s->state[i % STATE_BUFF_LEN].time,
//          fix16_to_float(log->pru_mem->s->state[i % STATE_BUFF_LEN].angle),
//          fix16_to_float(log->pru_mem->s->state[i % STATE_BUFF_LEN].angle_d));

    // Log
    if (logflag) {
      for(int i=log->cbuff->start; i<log->cbuff->start+size; i++){
        memset(log->cbuff->temp_buff, 0, TEMP_BUFF_LEN);
        PruSprintState(&log->pru_mem->s->state[i % STATE_BUFF_LEN],
                       log->cbuff->temp_buff);
        strcat(log->write_buff, log->cbuff->temp_buff);
      }


      // Write to file
      int len = strlen(log->write_buff);
      memcpy(log->addr + log->location, log->write_buff, len);
      log->location += len;
    }
    log->cbuff->start = (log->cbuff->start + size) % STATE_BUFF_LEN;
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
  return 0;
}

void LogCleanup(log_t* log) {
  free(log);
}

