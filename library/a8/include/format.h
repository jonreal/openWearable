#ifndef _FORMAT_H_
#define _FORMAT_H_

#include <stddef.h>
#include <stdint.h>
#include "mem_types.h"

// ---------------------------------------------------------------------------
//  Log schema (single source of truth, edited per app)
//
//  Each app describes its log as a flat table of fields: a column name, a type,
//  and the byte offset of that field inside state_t. The generic core
//  (library/a8/src/format_core.c) iterates this one table to pack the binary
//  record, emit the self-describing '#fields:' header, and render the -v debug
//  screen -- so an app edits only ow_schema[] (in its format.c), never the
//  pack/print/schema logic.
//
//  state_t is meant for shared access, not just logging, so log only the fields
//  you want, in any order:
//
//    const ow_field_t ow_schema[] = {
//      { "time",   OW_U32,   offsetof(state_t, time)        },
//      { "torque", OW_FIX16, offsetof(state_t, ctrl.torque) },
//    };
//    const size_t ow_schema_count = sizeof ow_schema / sizeof ow_schema[0];
//
//  Type tags match the host decoder's vocabulary (utils/logdecode). To add a
//  new type, add one row to OW_TYPE_META[] in format_core.c -- nowhere else.
// ---------------------------------------------------------------------------
typedef enum {
  OW_U8, OW_I8, OW_U16, OW_I16, OW_U32, OW_I32,
  OW_U64, OW_I64, OW_F32, OW_F64, OW_FIX16
} ow_type_t;

typedef struct {
  const char* name;    // column name -> '#fields:' token and debug header
  ow_type_t   type;    // drives record width, schema tag, and how to print
  uint16_t    offset;  // offsetof(state_t, field): where to read the value
} ow_field_t;

// Defined by each app in apps/<app>/format.c.
extern const ow_field_t ow_schema[];
extern const size_t      ow_schema_count;

// ---------------------------------------------------------------------------
//  Formatting API (implemented generically in format_core.c from ow_schema[],
//  except FormatSprintParams which stays app-specific).
// ---------------------------------------------------------------------------

// ASCII formatting (used by the -v debug screen path and as fallback).
void FormatSprintParams(const param_mem_t* param, char* buff);  // app-specific
void FormatSprintState(const state_t* st, char* buff);
void FormatSprintStateHeader(char* buff);
void FormatSprintPublishState(const state_t* st, char* buff);

// Binary log: the logger writes fixed-size packed records; the ASCII header
// (see log.c) carries the schema so the host decoder can rebuild the columns.
//   FormatLogRecordBytes : size in bytes of one packed record
//   FormatLogSchema      : writes the '#fields:' / '#record_bytes:' header lines
//   FormatLogRecord      : packs one state_t into a record (little-endian)
int  FormatLogRecordBytes(void);
void FormatLogSchema(char* buff);
void FormatLogRecord(const state_t* st, void* rec);

#endif
