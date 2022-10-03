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
#include "mem_types.h"
//#include "roshelper.h"

#define LOGSIZE         (4096 * 4096) * 20
#define TEMP_BUFF_LEN   1024
#define WRITE_BUFF_LEN  65536
#define MIN_STATE_REQ   2

// Circular Buffer Struct
typedef struct{
  uint32_t start;
  uint32_t end;
  char temp_buff[TEMP_BUFF_LEN];
} circbuff_t;

// Log File Struct
typedef struct{
  uint32_t fd;
  uint32_t location;
  char* addr;
  char write_buff[WRITE_BUFF_LEN];
  circbuff_t* cbuff;
  const pru_mem_t* pru_mem;
} log_t;

circbuff_t* LogNewCircBuff(void);
void LogCircBuffUpdate(log_t* log);
void LogDebugWriteState(const shared_mem_t* sm, circbuff_t* cb, char* buff);
log_t* LogInit(const pru_mem_t* pru_mem);
int LogNewFile(log_t* log, char* file);
void LogWriteStateToFile(log_t* log);
int LogSaveFile(log_t* log);
void LogCleanup(log_t* log);



//void LogWriteStateToFileAndPublish(log_t* log);


#endif /* _LOG_ */
