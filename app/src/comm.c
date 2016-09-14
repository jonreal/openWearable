#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/spi/spidev.h>
#include <sys/select.h>
#include <math.h>

#include "pinconfig.h"
#include "comm.h"
#include "control.h"
#include "tui.h"

/* THIS WHOLE FILE NEEDS REWRITE */
/* Comm struct */
udp_t *c_ptr;

/* ----------------------------------------------------------------------------
 * Function init_udp()
 * ------------------------------------------------------------------------- */
int init_udp(void)
{
  const char* SRV_IP = "192.168.7.1";
  const int PORT = 54321;

  /* Allocate */
  c_ptr = malloc(sizeof(udp_t));

  if ((c_ptr->sockfd=socket(AF_INET, SOCK_DGRAM,0)) == -1)
    printf("Socket Error\n");

  memset((char*) &c_ptr->servaddr, 0, sizeof(c_ptr->servaddr));
  c_ptr->servaddr.sin_family = AF_INET;
  c_ptr->servaddr.sin_addr.s_addr=inet_addr(SRV_IP);
  c_ptr->servaddr.sin_port=htons(PORT);
  bind(c_ptr->sockfd,
      (struct sockaddr *) &c_ptr->servaddr,sizeof(c_ptr->servaddr));

  c_ptr->timeout.tv_sec = 0;
  c_ptr->timeout.tv_usec = 500;

  while(1){
    FD_ZERO(&c_ptr->readfds);
    FD_SET(c_ptr->sockfd,&c_ptr->readfds);

    c_ptr->msg = "A";
    sendto(c_ptr->sockfd,c_ptr->msg,strlen(c_ptr->msg),0,
           (struct sockaddr*) &c_ptr->servaddr, sizeof(c_ptr->servaddr));

    c_ptr->rv = select(c_ptr->sockfd+1,&c_ptr->readfds,NULL,NULL,&c_ptr->timeout);
    if (c_ptr->rv==-1){
      printf("Error in Select\n");
      return -1;
    }
    else if (FD_ISSET(c_ptr->sockfd,&c_ptr->readfds)){
      int n = recvfrom(c_ptr->sockfd,c_ptr->recvBuff,1024,0,NULL,NULL);
      c_ptr->recvBuff[n] = 0;
      if (c_ptr->recvBuff[0] == 'A'){
        printf("UDP connection established.\n");
        break;
      }
    }
  }

  /* Set signal for reading udp */
  signal(SIGIO,udp_read_sig_cb);
	if (fcntl(c_ptr->sockfd,F_SETOWN,getpid()) < 0){
		perror("fcntl F_SETOWN");
		return -1;
	}
	if (fcntl(c_ptr->sockfd,F_SETFL,FASYNC) <0 ){
		perror("fcntl F_SETFL, FASYNC");
		return -1;
	}

  return 0;
}

/* ----------------------------------------------------------------------------
 * Function udp_read_sig_cb(*)
 * ------------------------------------------------------------------------- */
void udp_read_sig_cb(int signal)
{
  int n = recvfrom(c_ptr->sockfd,c_ptr->recvBuff,1024,0,NULL,NULL);
  c_ptr->recvBuff[n] = 0;

  if (c_ptr->recvBuff[0] == 'D'){
    set_pos_0(DEG2RAD(25.0));
//    ui_menu_cb();
  }
  else if (c_ptr->recvBuff[0] == 'P'){
    set_pos_0(DEG2RAD(-20.0));
  //  ui_menu_cb();
  }
  else if (c_ptr->recvBuff[0] == 'T'){
    if (!is_FF_on()){
      set_FB_cntrl(0);
      set_FF_cntrl(1);
    //  ui_menu_cb();
    }
    else if (is_FF_on()){
      set_FF_cntrl(0);
      set_pos_0(ANGLE_OFFSET);
    //  ui_menu_cb();
      set_FB_cntrl(1);
    }
  }
}


