#ifndef _FORMAT_H_
#define _FORMAT_H_

#include "mem_types.h"

// ASCII formatting (used by the -v debug screen path and as fallback).
void FormatSprintParams(const param_mem_t* param, char* buff);
void FormatSprintState(const state_t* st, char* buff);
void FormatSprintStateHeader(char* buff);
void FormatSprintPublishState(const state_t* st, char* buff);

// Binary log schema (per-app authority). The logger writes fixed-size packed
// records; the ASCII header (see log.c) carries the schema so a host-side
// decoder can reconstruct the columns.
//   FormatLogRecordBytes : size in bytes of one packed record
//   FormatLogSchema      : writes the '#fields:' / '#record_bytes:' header lines
//   FormatLogRecord      : packs one state_t into a record (little-endian)
int  FormatLogRecordBytes(void);
void FormatLogSchema(char* buff);
void FormatLogRecord(const state_t* st, void* rec);

#endif
