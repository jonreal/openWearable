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

// Define the PaRAM set structure to match hardware
//typedef struct {
//    volatile uint32_t opt;           // 0x00 - Options
//    volatile uint32_t src;           // 0x04 - Source address
//    volatile uint32_t a_b_cnt;       // 0x08 - A,B count
//    volatile uint32_t dst;           // 0x0C - Destination address
//    volatile uint32_t src_dst_bidx;  // 0x10 - Source, destination B index
//    volatile uint32_t link_bcnt;     // 0x14 - Link, B count reload
//    volatile uint32_t src_dst_cidx;  // 0x18 - Source, destination C index
//    volatile uint32_t ccnt;          // 0x1C - C count
//} edma_param_t;
//
//// DMA context 
//struct dma_context {
//    int dma_fd;                      // DMA device file descriptor
//    void *edma_base;                 // Mapped EDMA controller base address
//    void *dma_buf;                   // DMA buffer for source data
//    size_t dma_buf_size;             // Size of DMA buffer
//    edma_param_t *param_base;        // Pointer to PaRAM sets
//    volatile uint32_t *er_reg;       // Event Register
//    volatile uint32_t *ecr_reg;      // Event Clear Register
//    volatile uint32_t *esr_reg;      // Event Set Register
//    volatile uint32_t *eer_reg;      // Event Enable Register
//    volatile uint32_t *eecr_reg;     // Event Enable Clear Register
//    volatile uint32_t *eesr_reg;     // Event Enable Set Register
//    volatile uint32_t *ipr_reg;      // Interrupt Pending Register
//    volatile uint32_t *icr_reg;      // Interrupt Clear Register
//    int initialized;                 // Flag to indicate if DMA is initialized
//};

// BeagleBone AM335x EDMA controller definitions
//#define DMA_DEVICE "/dev/mem"
//#define EDMA_BASE_ADDR 0x49000000  // Base address of EDMA3 controller
//#define EDMA_SIZE 0x10000          // Size of memory region to map
//
//// EDMA register offsets
//#define ER_OFFSET 0x1000         // Event Register
//#define ECR_OFFSET 0x1008        // Event Clear Register
//#define ESR_OFFSET 0x1010        // Event Set Register
//#define EER_OFFSET 0x1020        // Event Enable Register
//#define EECR_OFFSET 0x1028       // Event Enable Clear Register
//#define EESR_OFFSET 0x1030       // Event Enable Set Register
//#define IPR_OFFSET 0x1068        // Interrupt Pending Register
//#define ICR_OFFSET 0x1070        // Interrupt Clear Register
//#define PARAM_OFFSET 0x4000      // PaRAM base
//#define PARAM_SIZE 32            // Each PaRAM entry is 32 bytes
//
//// PaRAM register offsets (within each 32-byte PaRAM set)
//#define OPT_OFFSET 0x00          // Options Parameter
//#define SRC_OFFSET 0x04          // Source Address
//#define A_B_CNT_OFFSET 0x08      // A and B Count
//#define DST_OFFSET 0x0C          // Destination Address
//#define SRC_DST_BIDX_OFFSET 0x10 // Source and Destination B Index
//#define LINK_BCNT_OFFSET 0x14    // Link and B Count Reload
//#define SRC_DST_CIDX_OFFSET 0x18 // Source and Destination C Index
//#define CCNT_OFFSET 0x1C         // C Count
//
//// Transfer complete bit
//#define OPT_TCINTEN (1 << 20)    // Transfer complete interrupt enable

// For simplicity, we'll use a predefined channel
//#define DMA_CHANNEL 8

// Max time to wait for DMA completion in microseconds
//#define DMA_TIMEOUT_US 1000000   // 1 second

// Function to initialize DMA
//static dma_context_t* init_dma(void) {
//    dma_context_t* ctx = malloc(sizeof(dma_context_t));
//    if (!ctx) {
//        fprintf(stderr, "Failed to allocate DMA context\n");
//        return NULL;
//    }
//    
//    // Initialize with default values
//    ctx->dma_fd = -1;
//    ctx->edma_base = NULL;
//    ctx->dma_buf = NULL;
//    ctx->dma_buf_size = 0;
//    ctx->initialized = 0;
//    
//    // Check if we're running on BeagleBone by looking for a key file
//    if (access("/sys/firmware/devicetree/base/model", F_OK) != 0) {
//        printf("Not running on BeagleBone, DMA disabled\n");
//        goto cleanup;
//    }
//    
//    // Open model file to check if we're on AM335x
//    FILE* model_file = fopen("/sys/firmware/devicetree/base/model", "r");
//    if (!model_file) {
//        printf("Could not determine platform, DMA disabled\n");
//        goto cleanup;
//    }
//    
//    char model[128];
//    fgets(model, sizeof(model), model_file);
//    fclose(model_file);
//    
//    // Only enable DMA on BeagleBone/AM335x platforms
//    if (strstr(model, "BeagleBone") == NULL) {
//        printf("Not running on BeagleBone, DMA disabled\n");
//        goto cleanup;
//    }
//    
//    // Open /dev/mem to access physical memory
//    ctx->dma_fd = open(DMA_DEVICE, O_RDWR | O_SYNC);
//    if (ctx->dma_fd < 0) {
//        fprintf(stderr, "Failed to open %s: %s\n", DMA_DEVICE, strerror(errno));
//        printf("DMA not available, using standard memcpy\n");
//        goto cleanup;
//    }
//    
//    // Map EDMA controller registers
//    ctx->edma_base = mmap(NULL, EDMA_SIZE, PROT_READ | PROT_WRITE, 
//                          MAP_SHARED, ctx->dma_fd, EDMA_BASE_ADDR);
//    if (ctx->edma_base == MAP_FAILED) {
//        fprintf(stderr, "Failed to map EDMA registers: %s\n", strerror(errno));
//        printf("DMA not available, using standard memcpy\n");
//        goto cleanup;
//    }
//    
//    // Set up pointers to key registers
//    ctx->er_reg = (volatile uint32_t*)((char*)ctx->edma_base + ER_OFFSET);
//    ctx->ecr_reg = (volatile uint32_t*)((char*)ctx->edma_base + ECR_OFFSET);
//    ctx->esr_reg = (volatile uint32_t*)((char*)ctx->edma_base + ESR_OFFSET);
//    ctx->eer_reg = (volatile uint32_t*)((char*)ctx->edma_base + EER_OFFSET);
//    ctx->eecr_reg = (volatile uint32_t*)((char*)ctx->edma_base + EECR_OFFSET);
//    ctx->eesr_reg = (volatile uint32_t*)((char*)ctx->edma_base + EESR_OFFSET);
//    ctx->ipr_reg = (volatile uint32_t*)((char*)ctx->edma_base + IPR_OFFSET);
//    ctx->icr_reg = (volatile uint32_t*)((char*)ctx->edma_base + ICR_OFFSET);
//    
//    // Get pointer to PaRAM base
//    ctx->param_base = (edma_param_t*)((char*)ctx->edma_base + PARAM_OFFSET);
//    
//    // Create buffer for DMA operations - use a smaller buffer size to ensure it fits
//    ctx->dma_buf_size = WRITE_BUFF_LEN;
//    
//    // Try to allocate DMA-friendly buffer with alignment
//    if (posix_memalign(&ctx->dma_buf, 128, ctx->dma_buf_size) != 0) {
//        fprintf(stderr, "Failed to allocate aligned DMA buffer\n");
//        printf("DMA not available, using standard memcpy\n");
//        goto cleanup;
//    }
//    
//    if (!ctx->dma_buf) {
//        fprintf(stderr, "Failed to allocate DMA buffer\n");
//        printf("DMA not available, using standard memcpy\n");
//        goto cleanup;
//    }
//    
//    // Zero out the buffer
//    memset(ctx->dma_buf, 0, ctx->dma_buf_size);
//    
//    // Initialize our DMA channel
//    // Disable event first
//    *(ctx->eecr_reg) = (1 << DMA_CHANNEL);
//    
//    // Clear any pending events
//    *(ctx->ecr_reg) = (1 << DMA_CHANNEL);
//    
//    // Set up PaRAM entry with defaults (will be updated per transfer)
//    ctx->param_base[DMA_CHANNEL].opt = OPT_TCINTEN;  // Transfer complete interrupt
//    ctx->param_base[DMA_CHANNEL].src = 0;            // Will be set during transfer
//    ctx->param_base[DMA_CHANNEL].dst = 0;            // Will be set during transfer
//    ctx->param_base[DMA_CHANNEL].a_b_cnt = 0;        // Will be set during transfer
//    ctx->param_base[DMA_CHANNEL].src_dst_bidx = 0;   // 1D transfer, no indexing
//    ctx->param_base[DMA_CHANNEL].link_bcnt = 0xFFFF; // No linking
//    ctx->param_base[DMA_CHANNEL].src_dst_cidx = 0;   // 1D transfer, no indexing
//    ctx->param_base[DMA_CHANNEL].ccnt = 1;           // 1D transfer
//    
//    // Enable DMA channel
//    *(ctx->eesr_reg) = (1 << DMA_CHANNEL);
//    
//    ctx->initialized = 1;
//    printf("DMA initialized successfully using EDMA channel %d\n", DMA_CHANNEL);
//    return ctx;
//    
//cleanup:
//    // Clean up if initialization fails
//    if (ctx->dma_buf) {
//        free(ctx->dma_buf);
//    }
//    
//    if (ctx->edma_base && ctx->edma_base != MAP_FAILED) {
//        munmap(ctx->edma_base, EDMA_SIZE);
//    }
//    
//    if (ctx->dma_fd >= 0) {
//        close(ctx->dma_fd);
//    }
//    
//    free(ctx);
//    return NULL;
//}
//
//// Function to perform DMA transfer
//static int dma_transfer(dma_context_t* ctx, void* src, void* dst, size_t len) {
//    if (!ctx || !ctx->initialized) {
//        return -1;
//    }
//    
//    // Make sure the length is valid
//    if (len == 0 || len > ctx->dma_buf_size) {
//        fprintf(stderr, "Invalid DMA transfer size: %zu\n", len);
//        return -1;
//    }
//    
//    // For small transfers, just use memcpy - DMA has overhead
//    if (len < 1024) {
//        memcpy(dst, src, len);
//        return 0;
//    }
//    
//    // Physical address check - memory must be in a DMA-compatible region
//    uintptr_t dst_addr = (uintptr_t)dst;
//    
//    // On AM335x, memory mapped files might not be DMA-accessible
//    // Let's check if the destination looks like a valid physical address
//    if (dst_addr < 0x80000000 || dst_addr > 0x90000000) {
//        // Likely not a physical address we can use with DMA
//        // Just use memcpy instead
//        memcpy(dst, src, len);
//        return 0;
//    }
//    
//    // Copy source data to our DMA buffer
//    // This step is needed because we need a DMA-accessible buffer
//    memcpy(ctx->dma_buf, src, len);
//    
//    // Synchronize memory - ensure data is written to physical memory
//    // before DMA reads it
//    __sync_synchronize();
//    
//    // Disable event first
//    *(ctx->eecr_reg) = (1 << DMA_CHANNEL);
//    
//    // Clear any pending events on our channel
//    *(ctx->ecr_reg) = (1 << DMA_CHANNEL);
//    *(ctx->icr_reg) = (1 << DMA_CHANNEL);
//    
//    // Set up transfer parameters
//    ctx->param_base[DMA_CHANNEL].src = (uint32_t)((uintptr_t)ctx->dma_buf);
//    ctx->param_base[DMA_CHANNEL].dst = (uint32_t)((uintptr_t)dst);
//    
//    // Set ACNT (number of bytes per array) and BCNT (number of arrays)
//    // For simplicity, we'll use a block size of 1024 bytes
//    uint32_t block_size = 1024;
//    uint32_t num_blocks = (len + block_size - 1) / block_size;
//    
//    if (num_blocks > 1) {
//        // ACNT=block_size, BCNT=num_blocks
//        ctx->param_base[DMA_CHANNEL].a_b_cnt = block_size | (num_blocks << 16);
//        ctx->param_base[DMA_CHANNEL].src_dst_bidx = block_size | (block_size << 16);
//    } else {
//        // Simple 1D transfer
//        ctx->param_base[DMA_CHANNEL].a_b_cnt = len | (1 << 16); // ACNT=len, BCNT=1
//        ctx->param_base[DMA_CHANNEL].src_dst_bidx = 0; // No indexing
//    }
//    
//    // Enable event
//    *(ctx->eesr_reg) = (1 << DMA_CHANNEL);
//    
//    // Trigger the transfer by setting the event
//    *(ctx->esr_reg) = (1 << DMA_CHANNEL);
//    
//    // Use a very short timeout to prevent freezes
//    #define SHORT_TIMEOUT_US 1000  // 1ms - if DMA doesn't complete quickly, use memcpy
//    
//    // Wait for transfer completion by polling the event register
//    struct timespec start_time, current_time;
//    clock_gettime(CLOCK_MONOTONIC, &start_time);
//    
//    // Set a maximum number of poll attempts
//    int max_polls = 100;
//    int poll_count = 0;
//    
//    while (poll_count < max_polls) {
//        poll_count++;
//        
//        // Check if transfer is complete by checking the event register
//        // A cleared event means the transfer is complete
//        if (!(*(ctx->er_reg) & (1 << DMA_CHANNEL))) {
//            // Success - DMA completed
//            return 0;
//        }
//        
//        // Brief pause to avoid hogging CPU
//        usleep(10); // 10 microseconds
//        
//        // Check for timeout
//        clock_gettime(CLOCK_MONOTONIC, &current_time);
//        uint64_t elapsed_us = (current_time.tv_sec - start_time.tv_sec) * 1000000 +
//                             (current_time.tv_nsec - start_time.tv_nsec) / 1000;
//        
//        if (elapsed_us > SHORT_TIMEOUT_US) {
//            // Timeout occurred
//            break;
//        }
//    }
//    
//    // If we get here, transfer didn't complete in time
//    // Don't print error to avoid flooding the console
//    // Just clear the events and fall back to memcpy
//    *(ctx->ecr_reg) = (1 << DMA_CHANNEL);
//    *(ctx->icr_reg) = (1 << DMA_CHANNEL);
//    *(ctx->eecr_reg) = (1 << DMA_CHANNEL);
//    
//    // Fall back to memcpy
//    memcpy(dst, src, len);
//    return -1;
//}
//
//// Function to cleanup DMA
//static void cleanup_dma(dma_context_t* ctx) {
//    if (!ctx) {
//        return;
//    }
//    
//    if (ctx->initialized) {
//        // Disable our DMA channel
//        if (ctx->eecr_reg) {
//            *(ctx->eecr_reg) = (1 << DMA_CHANNEL);
//        }
//        
//        // Clear any pending events/interrupts
//        if (ctx->ecr_reg) {
//            *(ctx->ecr_reg) = (1 << DMA_CHANNEL);
//        }
//        
//        if (ctx->icr_reg) {
//            *(ctx->icr_reg) = (1 << DMA_CHANNEL);
//        }
//    }
//    
//    // Free the DMA buffer
//    if (ctx->dma_buf) {
//        free(ctx->dma_buf);
//    }
//    
//    // Unmap the EDMA controller registers
//    if (ctx->edma_base && ctx->edma_base != MAP_FAILED) {
//        munmap(ctx->edma_base, EDMA_SIZE);
//    }
//    
//    // Close the device
//    if (ctx->dma_fd >= 0) {
//        close(ctx->dma_fd);
//    }
//    
//    free(ctx);
//}
////#include "roshelper.h"

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
  
  // Initialize DMA
  //log->dma_ctx = init_dma();
  //log->use_dma = (log->dma_ctx != NULL && log->dma_ctx->initialized);
  
  // Initialize stats flag
  //log->show_stats = 0;  // Don't show stats by default
  
  //if (log->use_dma) {
  //  printf("DMA logging enabled\n");
  //} else {
  //  printf("DMA initialization failed or not available, using standard memcpy\n");
  //}
  
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
  log->addr = mmap(
      0, LOGSIZE, PROT_WRITE, MAP_SHARED | MAP_POPULATE, log->fd, 0
  );
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
  FormatSprintParams(log->pru_mem->p, log->write_buff);
  len = strlen(log->write_buff);
  memcpy(log->addr + log->location, log->write_buff, len);
  log->location += len;
  log->write_buff[0] = '\0';

  // Log header
  FormatSprintStateHeader(log->write_buff);
  len = strlen(log->write_buff);
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
    // Measure formatting time
    clock_gettime(CLOCK_MONOTONIC, &format_time_start);
    
    for(int i=log->cbuff->start; i<log->cbuff->start+size; i++){
      memset(log->cbuff->temp_buff, 0, TEMP_BUFF_LEN);
      FormatSprintState(&log->pru_mem->s->state[i % STATE_BUFF_LEN],
                     log->cbuff->temp_buff);
      strcat(log->write_buff, log->cbuff->temp_buff);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &format_time_end);
    double format_ms = ((format_time_end.tv_sec - format_time_start.tv_sec) * 1000.0) + 
                      ((format_time_end.tv_nsec - format_time_start.tv_nsec) / 1000000.0);
    format_time_ms_total += format_ms;

    log->cbuff->start = (log->cbuff->start + size) % STATE_BUFF_LEN;

    // Write to file
    int len = strlen(log->write_buff);
    
    // Measure write time
    clock_gettime(CLOCK_MONOTONIC, &write_time_start);
    
    // Always use memcpy for the first few transfers to establish baseline
    static int transfer_count = 0;
    
//    if (transfer_count < 100) {
//        // Start with memcpy for the first 100 transfers
//        memcpy(log->addr + log->location, log->write_buff, len);
//        transfer_count++;
//    } else if (log->use_dma && log->dma_ctx && log->dma_ctx->initialized) {
//        // After establishing baseline, try DMA
//        dma_transfers++;
//        if (dma_transfer(log->dma_ctx, log->write_buff, log->addr + log->location, len) < 0) {
//            // Fall back to memcpy if DMA transfer fails
//            memcpy(log->addr + log->location, log->write_buff, len);
//            dma_errors++;
//            
//            // If we get too many consecutive errors, disable DMA
//            if (dma_errors > 20 && dma_errors == dma_transfers) {
//                printf("Too many DMA errors, disabling DMA\n");
//                log->use_dma = 0;
//            }
//        }
//    } else {
//        memcpy(log->addr + log->location, log->write_buff, len);
//    }

    memcpy(log->addr + log->location, log->write_buff, len);
    transfer_count++;
 
    clock_gettime(CLOCK_MONOTONIC, &write_time_end);
    double write_ms = (
        ((write_time_end.tv_sec - write_time_start.tv_sec) * 1000.0)
        + ((write_time_end.tv_nsec - write_time_start.tv_nsec) / 1000000.0)
    );
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
        //if (log->use_dma) {
        //  printf(" | DMA: %d transfers, %d errors", dma_transfers, dma_errors);
        //}
        
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
  // Clean up DMA resources if initialized
  //if (log->use_dma && log->dma_ctx) {
  //  cleanup_dma(log->dma_ctx);
  //  log->dma_ctx = NULL;
  //}
  
  free(log);
}


