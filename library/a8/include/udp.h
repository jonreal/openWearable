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

#ifndef _UDP_
#define _UDP_

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include "log.h"

#define REMOTE_SERVER_PORT 1500
#define MAX_PACKET_SIZE 1024

// Framed-binary scope protocol (see notes/scope-architecture.md + gui/scope.py).
// Header (little-endian, 12 bytes): magic[2]='OW' ver:u8 kind:u8 schema_id:u32 seq:u32
#define OW_SCOPE_VER          1
#define OW_SCOPE_KIND_DATA    0
#define OW_SCOPE_KIND_SCHEMA  1
#define OW_SCOPE_HEADER_SIZE  12

typedef struct {
  int sd, rc, i;
  struct sockaddr_in cliAddr, remoteServAddr;
  struct hostent *h;
  uint32_t seq;         // monotonic DATA-frame sequence (host-side drop detection)
  uint32_t schema_id;   // stable id of the active ow_schema[] layout
  char buff[MAX_PACKET_SIZE];
} udp_t;

udp_t* UdpInit(const char* myhostname);
void UdpPublish(const log_t* log, udp_t* udp);   // sends one binary DATA frame
void UdpPublishSchema(udp_t* udp);               // sends the '#fields:' SCHEMA frame

#endif
