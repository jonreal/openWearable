/* Copyright 2017-2019 Jonathan Realmuto

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

#include "udp.h"
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdint.h>
#include "format.h"

// FNV-1a 32-bit: a stable schema id from the '#fields:' text. The host listener
// keys DATA frames to the last SCHEMA frame's id, so only stability matters here
// (the algorithm need not match the host's).
static uint32_t ScopeFnv1a32(const char* s, size_t n) {
  uint32_t h = 2166136261u;
  for (size_t i = 0; i < n; i++) { h ^= (uint8_t) s[i]; h *= 16777619u; }
  return h;
}

// Write the 12-byte little-endian frame header; returns bytes written.
static int ScopeWriteHeader(uint8_t* buf, uint8_t kind,
                            uint32_t schema_id, uint32_t seq) {
  buf[0] = 'O'; buf[1] = 'W';
  buf[2] = OW_SCOPE_VER;
  buf[3] = kind;
  memcpy(buf + 4, &schema_id, 4);   // ARM A8 is little-endian == wire order
  memcpy(buf + 8, &seq, 4);
  return OW_SCOPE_HEADER_SIZE;
}


udp_t* UdpInit(const char* myhostname) {
  udp_t* udp = malloc(sizeof(udp_t));
  udp->buff[0] = '\0';

  udp->h = gethostbyname(myhostname);
  if (udp->h == NULL) {
    printf("unknown host\n");
    exit(1);
  }

  udp->remoteServAddr.sin_family = udp->h->h_addrtype;
  memcpy((char *) &udp->remoteServAddr.sin_addr.s_addr,udp->h->h_addr_list[0],
    udp->h->h_length);
  udp->remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  // socket creation
  udp->sd = socket(AF_INET,SOCK_DGRAM,0);
  if(udp->sd<0) {
    printf("cannot open socket \n");
    exit(1);
  }

  int broadcast = 1;
  if (setsockopt(udp->sd, SOL_SOCKET, SO_BROADCAST,
        &broadcast,sizeof broadcast) == -1) {
          perror("setsockopt (SO_BROADCAST)");
          exit(1);
  }

  // bind any port
  udp->cliAddr.sin_family = AF_INET;
  udp->cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  udp->cliAddr.sin_port = htons(0);

  udp->rc = bind(udp->sd, (struct sockaddr *) &(udp->cliAddr),
                  sizeof(udp->cliAddr));
  if(udp->rc<0) {
    printf("cannot bind port\n");
    exit(1);
  }

  // Precompute the scope schema id from the active '#fields:' layout.
  FormatLogSchema(udp->buff);
  udp->schema_id = ScopeFnv1a32(udp->buff, strlen(udp->buff));
  udp->seq = 0;

  return udp;
}

// One binary DATA frame: [header][FormatLogRecord bytes] for the newest ring slot.
void UdpPublish(const log_t* log, udp_t* udp) {
  uint8_t* p = (uint8_t*) udp->buff;
  int n = ScopeWriteHeader(p, OW_SCOPE_KIND_DATA, udp->schema_id, udp->seq);
  int i = log->cbuff->end;   // most-recent ring slot
  FormatLogRecord(&log->pru_mem->s->state[i % STATE_BUFF_LEN], p + n);
  int len = n + FormatLogRecordBytes();
  udp->rc = sendto(udp->sd, udp->buff, len, 0,
      (struct sockaddr *) &(udp->remoteServAddr), sizeof(udp->remoteServAddr));
  udp->seq++;
}

// One SCHEMA frame: [header][ASCII '#fields:' text] so late-joining listeners
// self-configure. Sent at startup and as a low-rate heartbeat (see uiloop).
void UdpPublishSchema(udp_t* udp) {
  uint8_t* p = (uint8_t*) udp->buff;
  int n = ScopeWriteHeader(p, OW_SCOPE_KIND_SCHEMA, udp->schema_id, 0);
  FormatLogSchema((char*) (p + n));
  int len = n + (int) strlen((char*) (p + n));
  udp->rc = sendto(udp->sd, udp->buff, len, 0,
      (struct sockaddr *) &(udp->remoteServAddr), sizeof(udp->remoteServAddr));
}

