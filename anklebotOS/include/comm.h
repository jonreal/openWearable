#ifndef _COMM_H_
#define _COMM_H

/* structures ------------------------------------------------------------- */
typedef struct{
  int sockfd;
  struct sockaddr_in servaddr;
  char recvBuff[1024];
  char* msg;
  int rv;
  struct timeval timeout;
  fd_set readfds;
} udp_t;

/* globals ---------------------------------------------------------------- */
extern const int debug;

/* prototypes ------------------------------------------------------------- */
int init_udp(void);
void udp_read_sig_cb(int signal);


#endif
