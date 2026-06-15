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

#ifndef _DMA_H_
#define _DMA_H_

#include <stddef.h>

// Opaque DMA backend context. The implementation (dma.c) is board-specific:
// AM335x uses the EDMA3 controller; other SoCs provide their own backend behind
// this same interface (e.g. J721E UDMA), so callers (log.c) stay board-agnostic.
typedef struct dma_context dma_context_t;

// Initialize the DMA backend. 'bufsize' is the largest single transfer the
// caller will request. Returns NULL (caller should fall back to memcpy) when
// DMA is unavailable on this platform.
dma_context_t* DmaInit(size_t bufsize);

// Non-zero if 'ctx' is a ready DMA backend (NULL-safe).
int DmaInitialized(const dma_context_t* ctx);

// Copy 'len' bytes src->dst via DMA. Returns 0 on success, <0 on failure. On
// failure the backend has already fallen back to memcpy, so the bytes are
// copied either way.
int DmaTransfer(dma_context_t* ctx, void* src, void* dst, size_t len);

// Release the DMA backend (NULL-safe).
void DmaCleanup(dma_context_t* ctx);

#endif /* _DMA_H_ */
