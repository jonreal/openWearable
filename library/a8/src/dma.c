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

/* dma.c - AM335x EDMA3 backend for the dma.h interface.
 *
 * This is the board-specific DMA implementation: it maps the AM335x EDMA3
 * controller via /dev/mem and uses one PaRAM set to accelerate large block
 * copies (the logger's buffer -> mmap'd file write). On any non-AM335x
 * platform DmaInit() returns NULL and callers fall back to memcpy. A J721E
 * (UDMA) backend would replace this file behind the same interface. */

#include "dma.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>

// Define the PaRAM set structure to match hardware
typedef struct {
    volatile uint32_t opt;           // 0x00 - Options
    volatile uint32_t src;           // 0x04 - Source address
    volatile uint32_t a_b_cnt;       // 0x08 - A,B count
    volatile uint32_t dst;           // 0x0C - Destination address
    volatile uint32_t src_dst_bidx;  // 0x10 - Source, destination B index
    volatile uint32_t link_bcnt;     // 0x14 - Link, B count reload
    volatile uint32_t src_dst_cidx;  // 0x18 - Source, destination C index
    volatile uint32_t ccnt;          // 0x1C - C count
} edma_param_t;

// DMA context
struct dma_context {
    int dma_fd;                      // DMA device file descriptor
    void *edma_base;                 // Mapped EDMA controller base address
    void *dma_buf;                   // DMA buffer for source data
    size_t dma_buf_size;             // Size of DMA buffer
    edma_param_t *param_base;        // Pointer to PaRAM sets
    volatile uint32_t *er_reg;       // Event Register
    volatile uint32_t *ecr_reg;      // Event Clear Register
    volatile uint32_t *esr_reg;      // Event Set Register
    volatile uint32_t *eer_reg;      // Event Enable Register
    volatile uint32_t *eecr_reg;     // Event Enable Clear Register
    volatile uint32_t *eesr_reg;     // Event Enable Set Register
    volatile uint32_t *ipr_reg;      // Interrupt Pending Register
    volatile uint32_t *icr_reg;      // Interrupt Clear Register
    int initialized;                 // Flag to indicate if DMA is initialized
};

// BeagleBone AM335x EDMA controller definitions
#define DMA_DEVICE "/dev/mem"
#define EDMA_BASE_ADDR 0x49000000  // Base address of EDMA3 controller
#define EDMA_SIZE 0x10000          // Size of memory region to map

// EDMA register offsets
#define ER_OFFSET 0x1000         // Event Register
#define ECR_OFFSET 0x1008        // Event Clear Register
#define ESR_OFFSET 0x1010        // Event Set Register
#define EER_OFFSET 0x1020        // Event Enable Register
#define EECR_OFFSET 0x1028       // Event Enable Clear Register
#define EESR_OFFSET 0x1030       // Event Enable Set Register
#define IPR_OFFSET 0x1068        // Interrupt Pending Register
#define ICR_OFFSET 0x1070        // Interrupt Clear Register
#define PARAM_OFFSET 0x4000      // PaRAM base
#define PARAM_SIZE 32            // Each PaRAM entry is 32 bytes

// Transfer complete bit
#define OPT_TCINTEN (1 << 20)    // Transfer complete interrupt enable

// For simplicity, we'll use a predefined channel
#define DMA_CHANNEL 8

dma_context_t* DmaInit(size_t bufsize) {
    dma_context_t* ctx = malloc(sizeof(dma_context_t));
    if (!ctx) {
        fprintf(stderr, "Failed to allocate DMA context\n");
        return NULL;
    }

    // Initialize with default values
    ctx->dma_fd = -1;
    ctx->edma_base = NULL;
    ctx->dma_buf = NULL;
    ctx->dma_buf_size = 0;
    ctx->initialized = 0;

    // Check if we're running on BeagleBone by looking for a key file
    if (access("/sys/firmware/devicetree/base/model", F_OK) != 0) {
        printf("Not running on BeagleBone, DMA disabled\n");
        goto cleanup;
    }

    // Open model file to check if we're on AM335x
    FILE* model_file = fopen("/sys/firmware/devicetree/base/model", "r");
    if (!model_file) {
        printf("Could not determine platform, DMA disabled\n");
        goto cleanup;
    }

    char model[128];
    fgets(model, sizeof(model), model_file);
    fclose(model_file);

    // Only enable DMA on BeagleBone/AM335x platforms
    if (strstr(model, "BeagleBone") == NULL) {
        printf("Not running on BeagleBone, DMA disabled\n");
        goto cleanup;
    }

    // Open /dev/mem to access physical memory
    ctx->dma_fd = open(DMA_DEVICE, O_RDWR | O_SYNC);
    if (ctx->dma_fd < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", DMA_DEVICE, strerror(errno));
        printf("DMA not available, using standard memcpy\n");
        goto cleanup;
    }

    // Map EDMA controller registers
    ctx->edma_base = mmap(NULL, EDMA_SIZE, PROT_READ | PROT_WRITE,
                          MAP_SHARED, ctx->dma_fd, EDMA_BASE_ADDR);
    if (ctx->edma_base == MAP_FAILED) {
        fprintf(stderr, "Failed to map EDMA registers: %s\n", strerror(errno));
        printf("DMA not available, using standard memcpy\n");
        goto cleanup;
    }

    // Set up pointers to key registers
    ctx->er_reg = (volatile uint32_t*)((char*)ctx->edma_base + ER_OFFSET);
    ctx->ecr_reg = (volatile uint32_t*)((char*)ctx->edma_base + ECR_OFFSET);
    ctx->esr_reg = (volatile uint32_t*)((char*)ctx->edma_base + ESR_OFFSET);
    ctx->eer_reg = (volatile uint32_t*)((char*)ctx->edma_base + EER_OFFSET);
    ctx->eecr_reg = (volatile uint32_t*)((char*)ctx->edma_base + EECR_OFFSET);
    ctx->eesr_reg = (volatile uint32_t*)((char*)ctx->edma_base + EESR_OFFSET);
    ctx->ipr_reg = (volatile uint32_t*)((char*)ctx->edma_base + IPR_OFFSET);
    ctx->icr_reg = (volatile uint32_t*)((char*)ctx->edma_base + ICR_OFFSET);

    // Get pointer to PaRAM base
    ctx->param_base = (edma_param_t*)((char*)ctx->edma_base + PARAM_OFFSET);

    // Create buffer for DMA operations
    ctx->dma_buf_size = bufsize;

    // Try to allocate DMA-friendly buffer with alignment
    if (posix_memalign(&ctx->dma_buf, 128, ctx->dma_buf_size) != 0) {
        fprintf(stderr, "Failed to allocate aligned DMA buffer\n");
        printf("DMA not available, using standard memcpy\n");
        goto cleanup;
    }

    if (!ctx->dma_buf) {
        fprintf(stderr, "Failed to allocate DMA buffer\n");
        printf("DMA not available, using standard memcpy\n");
        goto cleanup;
    }

    // Zero out the buffer
    memset(ctx->dma_buf, 0, ctx->dma_buf_size);

    // Initialize our DMA channel
    // Disable event first
    *(ctx->eecr_reg) = (1 << DMA_CHANNEL);

    // Clear any pending events
    *(ctx->ecr_reg) = (1 << DMA_CHANNEL);

    // Set up PaRAM entry with defaults (will be updated per transfer)
    ctx->param_base[DMA_CHANNEL].opt = OPT_TCINTEN;  // Transfer complete interrupt
    ctx->param_base[DMA_CHANNEL].src = 0;            // Will be set during transfer
    ctx->param_base[DMA_CHANNEL].dst = 0;            // Will be set during transfer
    ctx->param_base[DMA_CHANNEL].a_b_cnt = 0;        // Will be set during transfer
    ctx->param_base[DMA_CHANNEL].src_dst_bidx = 0;   // 1D transfer, no indexing
    ctx->param_base[DMA_CHANNEL].link_bcnt = 0xFFFF; // No linking
    ctx->param_base[DMA_CHANNEL].src_dst_cidx = 0;   // 1D transfer, no indexing
    ctx->param_base[DMA_CHANNEL].ccnt = 1;           // 1D transfer

    // Enable DMA channel
    *(ctx->eesr_reg) = (1 << DMA_CHANNEL);

    ctx->initialized = 1;
    printf("DMA initialized successfully using EDMA channel %d\n", DMA_CHANNEL);
    return ctx;

cleanup:
    // Clean up if initialization fails
    if (ctx->dma_buf) {
        free(ctx->dma_buf);
    }

    if (ctx->edma_base && ctx->edma_base != MAP_FAILED) {
        munmap(ctx->edma_base, EDMA_SIZE);
    }

    if (ctx->dma_fd >= 0) {
        close(ctx->dma_fd);
    }

    free(ctx);
    return NULL;
}

int DmaInitialized(const dma_context_t* ctx) {
    return ctx && ctx->initialized;
}

int DmaTransfer(dma_context_t* ctx, void* src, void* dst, size_t len) {
    if (!ctx || !ctx->initialized) {
        return -1;
    }

    // Make sure the length is valid
    if (len == 0 || len > ctx->dma_buf_size) {
        fprintf(stderr, "Invalid DMA transfer size: %zu\n", len);
        return -1;
    }

    // For small transfers, just use memcpy - DMA has overhead
    if (len < 1024) {
        memcpy(dst, src, len);
        return 0;
    }

    // Physical address check - memory must be in a DMA-compatible region
    uintptr_t dst_addr = (uintptr_t)dst;

    // On AM335x, memory mapped files might not be DMA-accessible
    // Let's check if the destination looks like a valid physical address
    if (dst_addr < 0x80000000 || dst_addr > 0x90000000) {
        // Likely not a physical address we can use with DMA
        // Just use memcpy instead
        memcpy(dst, src, len);
        return 0;
    }

    // Copy source data to our DMA buffer
    // This step is needed because we need a DMA-accessible buffer
    memcpy(ctx->dma_buf, src, len);

    // Synchronize memory - ensure data is written to physical memory
    // before DMA reads it
    __sync_synchronize();

    // Disable event first
    *(ctx->eecr_reg) = (1 << DMA_CHANNEL);

    // Clear any pending events on our channel
    *(ctx->ecr_reg) = (1 << DMA_CHANNEL);
    *(ctx->icr_reg) = (1 << DMA_CHANNEL);

    // Set up transfer parameters
    ctx->param_base[DMA_CHANNEL].src = (uint32_t)((uintptr_t)ctx->dma_buf);
    ctx->param_base[DMA_CHANNEL].dst = (uint32_t)((uintptr_t)dst);

    // Set ACNT (number of bytes per array) and BCNT (number of arrays)
    // For simplicity, we'll use a block size of 1024 bytes
    uint32_t block_size = 1024;
    uint32_t num_blocks = (len + block_size - 1) / block_size;

    if (num_blocks > 1) {
        // ACNT=block_size, BCNT=num_blocks
        ctx->param_base[DMA_CHANNEL].a_b_cnt = block_size | (num_blocks << 16);
        ctx->param_base[DMA_CHANNEL].src_dst_bidx = block_size | (block_size << 16);
    } else {
        // Simple 1D transfer
        ctx->param_base[DMA_CHANNEL].a_b_cnt = len | (1 << 16); // ACNT=len, BCNT=1
        ctx->param_base[DMA_CHANNEL].src_dst_bidx = 0; // No indexing
    }

    // Enable event
    *(ctx->eesr_reg) = (1 << DMA_CHANNEL);

    // Trigger the transfer by setting the event
    *(ctx->esr_reg) = (1 << DMA_CHANNEL);

    // Use a very short timeout to prevent freezes
    #define SHORT_TIMEOUT_US 1000  // 1ms - if DMA doesn't complete quickly, use memcpy

    // Wait for transfer completion by polling the event register
    struct timespec start_time, current_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Set a maximum number of poll attempts
    int max_polls = 100;
    int poll_count = 0;

    while (poll_count < max_polls) {
        poll_count++;

        // Check if transfer is complete by checking the event register
        // A cleared event means the transfer is complete
        if (!(*(ctx->er_reg) & (1 << DMA_CHANNEL))) {
            // Success - DMA completed
            return 0;
        }

        // Brief pause to avoid hogging CPU
        usleep(10); // 10 microseconds

        // Check for timeout
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        uint64_t elapsed_us = (current_time.tv_sec - start_time.tv_sec) * 1000000 +
                             (current_time.tv_nsec - start_time.tv_nsec) / 1000;

        if (elapsed_us > SHORT_TIMEOUT_US) {
            // Timeout occurred
            break;
        }
    }

    // If we get here, transfer didn't complete in time. Clear events and fall
    // back to memcpy (no console spam).
    *(ctx->ecr_reg) = (1 << DMA_CHANNEL);
    *(ctx->icr_reg) = (1 << DMA_CHANNEL);
    *(ctx->eecr_reg) = (1 << DMA_CHANNEL);

    memcpy(dst, src, len);
    return -1;
}

void DmaCleanup(dma_context_t* ctx) {
    if (!ctx) {
        return;
    }

    if (ctx->initialized) {
        // Disable our DMA channel
        if (ctx->eecr_reg) {
            *(ctx->eecr_reg) = (1 << DMA_CHANNEL);
        }

        // Clear any pending events/interrupts
        if (ctx->ecr_reg) {
            *(ctx->ecr_reg) = (1 << DMA_CHANNEL);
        }

        if (ctx->icr_reg) {
            *(ctx->icr_reg) = (1 << DMA_CHANNEL);
        }
    }

    // Free the DMA buffer
    if (ctx->dma_buf) {
        free(ctx->dma_buf);
    }

    // Unmap the EDMA controller registers
    if (ctx->edma_base && ctx->edma_base != MAP_FAILED) {
        munmap(ctx->edma_base, EDMA_SIZE);
    }

    // Close the device
    if (ctx->dma_fd >= 0) {
        close(ctx->dma_fd);
    }

    free(ctx);
}
