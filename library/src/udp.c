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
#include "format.h"


udp_t* UdpInit(void) {
  udp_t* udp = malloc(sizeof(udp_t));
  udp->buff[0] = '\0';

  // get ip
  udp->h = gethostbyname("deepthought.local");
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

  return udp;
}

void UdpPublish(const log_t* log, udp_t* udp) {
  udp->buff[0] = '\0';
  int i = log->cbuff->end; // print from end of buff
  FormatSprintPublishState(&log->pru_mem->s->state[i % STATE_BUFF_LEN],
                          udp->buff);
  udp->rc = sendto(udp->sd, udp->buff, MAX_PACKET_SIZE, 0,
      (struct sockaddr *) &(udp->remoteServAddr),sizeof(udp->remoteServAddr));
}

