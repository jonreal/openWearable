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
#include "log.h"

#define REMOTE_SERVER_PORT 1500
#define MAX_PACKET_SIZE 1024

typedef struct {
  int sd, rc, i;
  struct sockaddr_in cliAddr, remoteServAddr;
  struct hostent *h;
  char buff[MAX_PACKET_SIZE];
} udp_t;

udp_t* UdpInit(const char* myhostname);
void UdpPublish(const log_t* log, udp_t* udp);

#endif
