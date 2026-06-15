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

/* logdecode - openWearable binary log -> CSV
 *
 *   Usage: logdecode <logfile.bin>  > out.csv
 *
 * Reads the self-describing ASCII header that openWearable's logger writes
 * (see library/a8/src/log.c):
 *
 *   #openWearable-log v1
 *   #date: ...
 *   #fs_hz: 1000
 *   #fields: time:u32,pru0var:u32,...
 *   #record_bytes: 16
 *   #DATA
 *   <raw fixed-size little-endian records>
 *
 * then streams the binary records out as CSV (one row per record). The field
 * order/types in '#fields:' must match the packing in the app's format.c
 * FormatLogRecord(). Supported types: u8 i8 u16 i16 u32 i32 u64 i64 f32 f64
 * and fix16 (Q16.16 fixed-point -> decoded as float).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_FIELDS 256
#define MAX_NAME   64

typedef struct { char name[MAX_NAME]; char type[16]; } field_t;

static int type_size(const char* t) {
  if (!strcmp(t, "u8")  || !strcmp(t, "i8"))  return 1;
  if (!strcmp(t, "u16") || !strcmp(t, "i16")) return 2;
  if (!strcmp(t, "u32") || !strcmp(t, "i32") ||
      !strcmp(t, "f32") || !strcmp(t, "fix16")) return 4;
  if (!strcmp(t, "u64") || !strcmp(t, "i64") || !strcmp(t, "f64")) return 8;
  return 0;  /* unknown */
}

static void print_value(const char* t, const uint8_t* p) {
  if      (!strcmp(t, "u8"))  printf("%u", (unsigned) *p);
  else if (!strcmp(t, "i8"))  printf("%d", (int) *(const int8_t*) p);
  else if (!strcmp(t, "u16")) { uint16_t v; memcpy(&v, p, 2); printf("%u", v); }
  else if (!strcmp(t, "i16")) { int16_t  v; memcpy(&v, p, 2); printf("%d", v); }
  else if (!strcmp(t, "u32")) { uint32_t v; memcpy(&v, p, 4); printf("%u", v); }
  else if (!strcmp(t, "i32")) { int32_t  v; memcpy(&v, p, 4); printf("%d", v); }
  else if (!strcmp(t, "f32")) { float    v; memcpy(&v, p, 4); printf("%g", v); }
  else if (!strcmp(t, "f64")) { double   v; memcpy(&v, p, 8); printf("%g", v); }
  else if (!strcmp(t, "fix16")) { int32_t v; memcpy(&v, p, 4); printf("%g", v / 65536.0); }
  else if (!strcmp(t, "u64")) { uint64_t v; memcpy(&v, p, 8); printf("%llu", (unsigned long long) v); }
  else if (!strcmp(t, "i64")) { int64_t  v; memcpy(&v, p, 8); printf("%lld", (long long) v); }
  else printf("?");
}

int main(int argc, char** argv) {
  if (argc < 2) { fprintf(stderr, "usage: %s <logfile.bin>\n", argv[0]); return 1; }

  FILE* f = fopen(argv[1], "rb");
  if (!f) { perror("open"); return 1; }

  field_t fields[MAX_FIELDS];
  int nf = 0, recbytes = 0, have_data = 0;
  char line[2048];

  /* Parse the ASCII header line-by-line until the "#DATA" sentinel. */
  while (fgets(line, sizeof(line), f)) {
    if (!strncmp(line, "#DATA", 5)) { have_data = 1; break; }

    char* p;
    if ((p = strstr(line, "#fields:")) != NULL) {
      p += 8;
      for (char* tok = strtok(p, ", \t\r\n"); tok && nf < MAX_FIELDS;
           tok = strtok(NULL, ", \t\r\n")) {
        char* colon = strchr(tok, ':');
        if (!colon) continue;
        *colon = '\0';
        snprintf(fields[nf].name, MAX_NAME, "%s", tok);
        snprintf(fields[nf].type, sizeof(fields[nf].type), "%s", colon + 1);
        nf++;
      }
    } else if ((p = strstr(line, "#record_bytes:")) != NULL) {
      recbytes = atoi(p + 14);
    }
  }

  if (!have_data || nf == 0 || recbytes <= 0) {
    fprintf(stderr, "logdecode: bad/missing header (fields=%d record_bytes=%d)\n",
            nf, recbytes);
    fclose(f);
    return 1;
  }

  /* CSV column header. */
  for (int i = 0; i < nf; i++) printf("%s%s", i ? "," : "", fields[i].name);
  printf("\n");

  /* Stream records -> CSV rows. */
  uint8_t* rec = malloc((size_t) recbytes);
  if (!rec) { fclose(f); return 1; }
  unsigned long rows = 0;
  while (fread(rec, 1, (size_t) recbytes, f) == (size_t) recbytes) {
    int off = 0;
    for (int i = 0; i < nf; i++) {
      if (i) printf(",");
      print_value(fields[i].type, rec + off);
      off += type_size(fields[i].type);
    }
    printf("\n");
    rows++;
  }

  free(rec);
  fclose(f);
  fprintf(stderr, "logdecode: %lu records, %d fields, %d bytes/record\n",
          rows, nf, recbytes);
  return 0;
}
