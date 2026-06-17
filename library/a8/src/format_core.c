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

// Generic log/format core. Every Format* function below is driven by the app's
// ow_schema[] table (apps/<app>/format.c) by iterating it once per record, so
// adding/removing a logged column means editing only that table -- the packing,
// the '#fields:' header, and the -v debug screen all follow. The per-field cost
// is a small loop; if a hot path ever needs it, ow_schema[] can be compiled into
// a coalesced memcpy plan at init behind these same signatures, no API change.

#include "format.h"
#include "fix16.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Per-type metadata, indexed by ow_type_t. One row per supported type; the tag
// strings are the vocabulary the host decoder (utils/logdecode) parses back.
// Add a type here (and to ow_type_t / FormatSprintValue) -- nowhere else.
static const struct {
  uint8_t     width;   // bytes in the packed record
  const char* tag;     // '#fields:' type token
} OW_TYPE_META[] = {
  [OW_U8]    = { 1, "u8"    },
  [OW_I8]    = { 1, "i8"    },
  [OW_U16]   = { 2, "u16"   },
  [OW_I16]   = { 2, "i16"   },
  [OW_U32]   = { 4, "u32"   },
  [OW_I32]   = { 4, "i32"   },
  [OW_U64]   = { 8, "u64"   },
  [OW_I64]   = { 8, "i64"   },
  [OW_F32]   = { 4, "f32"   },
  [OW_F64]   = { 8, "f64"   },
  [OW_FIX16] = { 4, "fix16" },
};

static inline const uint8_t* FieldPtr(const state_t* st, const ow_field_t* f) {
  return (const uint8_t*) st + f->offset;
}

// Render one field value as ASCII (debug screen). Mirrors the host decoder's
// print_value() so the -v screen reads like the eventual CSV; fix16 is scaled
// via fix16_to_float (the binary record still stores the raw Q16.16 int32).
// Returns the number of characters written.
static int FormatSprintValue(char* buff, ow_type_t t, const uint8_t* p) {
  switch (t) {
    case OW_U8:  return sprintf(buff, "%u", (unsigned) *p);
    case OW_I8:  return sprintf(buff, "%d", (int) *(const int8_t*) p);
    case OW_U16: { uint16_t v; memcpy(&v, p, 2); return sprintf(buff, "%u", v); }
    case OW_I16: { int16_t  v; memcpy(&v, p, 2); return sprintf(buff, "%d", v); }
    case OW_U32: { uint32_t v; memcpy(&v, p, 4); return sprintf(buff, "%u", v); }
    case OW_I32: { int32_t  v; memcpy(&v, p, 4); return sprintf(buff, "%d", v); }
    case OW_U64: { uint64_t v; memcpy(&v, p, 8); return sprintf(buff, "%llu", (unsigned long long) v); }
    case OW_I64: { int64_t  v; memcpy(&v, p, 8); return sprintf(buff, "%lld", (long long) v); }
    case OW_F32: { float    v; memcpy(&v, p, 4); return sprintf(buff, "%g", v); }
    case OW_F64: { double   v; memcpy(&v, p, 8); return sprintf(buff, "%g", v); }
    case OW_FIX16: { fix16_t v; memcpy(&v, p, 4); return sprintf(buff, "%g", fix16_to_float(v)); }
  }
  return sprintf(buff, "?");
}

// ---------------------------------------------------------------------------
//  Binary log path
// ---------------------------------------------------------------------------
int FormatLogRecordBytes(void) {
  int n = 0;
  for (size_t i = 0; i < ow_schema_count; i++)
    n += OW_TYPE_META[ow_schema[i].type].width;
  return n;
}

void FormatLogRecord(const state_t* st, void* rec) {
  uint8_t* wp = (uint8_t*) rec;
  for (size_t i = 0; i < ow_schema_count; i++) {
    uint8_t w = OW_TYPE_META[ow_schema[i].type].width;
    memcpy(wp, FieldPtr(st, &ow_schema[i]), w);   // little-endian host == file
    wp += w;
  }
}

void FormatLogSchema(char* buff) {
  int n = sprintf(buff, "\n#fields: ");
  for (size_t i = 0; i < ow_schema_count; i++)
    n += sprintf(buff + n, "%s%s:%s",
                 i ? "," : "",                     // comma-separated, no trailer
                 ow_schema[i].name,
                 OW_TYPE_META[ow_schema[i].type].tag);
  sprintf(buff + n, "\n#record_bytes: %d\n", FormatLogRecordBytes());
}

// ---------------------------------------------------------------------------
//  ASCII debug path (-v screen). Header names now match the log field names.
// ---------------------------------------------------------------------------
void FormatSprintStateHeader(char* buff) {
  int n = sprintf(buff, "\n# ");
  for (size_t i = 0; i < ow_schema_count; i++)
    n += sprintf(buff + n, "%s\t", ow_schema[i].name);
  sprintf(buff + n, "\n");
}

void FormatSprintState(const state_t* st, char* buff) {
  int n = 0;
  for (size_t i = 0; i < ow_schema_count; i++) {
    n += FormatSprintValue(buff + n, ow_schema[i].type, FieldPtr(st, &ow_schema[i]));
    n += sprintf(buff + n, "\t");
  }
  sprintf(buff + n, "\n");
}

void FormatSprintPublishState(const state_t* st, char* buff) {
  FormatSprintState(st, buff);
}
