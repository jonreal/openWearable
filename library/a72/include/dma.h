/* Copyright 2025 Jonathan Realmuto

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


