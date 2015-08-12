#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "common.h"
#include "control.h"
#include "spi.h"

SPI_t *SPI;

int init_spi(void)
{

  SPI = malloc(sizeof(SPI_t));
  SPI->msg = malloc(sizeof(struct spi_ioc_transfer));

  SPI->mode = 0;
  SPI->msg->speed_hz = (__u32) 5000000;
  SPI->msg->delay_usecs = (__u16) 0;
  SPI->msg->bits_per_word = (__u8) 8;
  SPI->msg->cs_change = (__u8) 0;
  SPI->msg->pad = (__u8) 0;

  /* Load Device tree and open SPI device */
  if(load_device_tree("BB-SPIDEV0") != 1){
    printf("Unable to load spi device tree.\n");
    return -1;
  }
  SPI->fd = open("/dev/spidev1.0", O_RDWR);
  if(SPI->fd < 0){
    printf("Can't open spi device.\n");
    return -1;
  }

  /* Mode */
  if(ioctl(SPI->fd, SPI_IOC_WR_MODE, &SPI->mode) == -1){
    printf("Can't set spi mode.\n");
    return -1;
  }
  if(ioctl(SPI->fd, SPI_IOC_RD_MODE, &SPI->mode) == -1){
    printf("Can't get spi mode\n");
    return -1;
  }

   /* Bits per word */
  if(ioctl(SPI->fd, SPI_IOC_WR_BITS_PER_WORD, &SPI->msg->bits_per_word) == -1){
    printf("Can't set bits per word\n");
    return -1;
  }
  if(ioctl(SPI->fd, SPI_IOC_RD_BITS_PER_WORD, &SPI->msg->bits_per_word) == -1){
    printf("Can't get bits per word\n");
    return -1;
  }

  /* Max speed in Hz */
  if(ioctl(SPI->fd, SPI_IOC_WR_MAX_SPEED_HZ, &SPI->msg->speed_hz) == -1){
    printf("Can't set max speed\n");
    return -1;
  }
  if(ioctl(SPI->fd, SPI_IOC_RD_MAX_SPEED_HZ, &SPI->msg->speed_hz) == -1){
    printf("Can't get max speed\n");
    return -1;
  }

  add_func_to_cleanup(&spi_cleanup);

  printf("SPI initialized.\n");
  printf("\tspi mode: %d\n", SPI->mode);
	printf("\tbits per word: %d\n", SPI->msg->bits_per_word);
	printf("\tmax speed: %d Hz\n", SPI->msg->speed_hz);

  return 0;
}

int spi_transfer(uint8_t tx[], uint8_t* rx)
{
  int err;

  SPI->msg->tx_buf = (unsigned long) tx;
  SPI->msg->rx_buf = (unsigned long) rx;
  SPI->msg->len = ARRAY_SIZE(tx);

  err = ioctl(SPI->fd, SPI_IOC_MESSAGE(1), SPI->msg);
  if(err < 1){
    printf("Can't send spi message:\n");
    printf("err = %i\n", err);
    return -1;
  }
   return 0;
}

int spi_cleanup(void)
{
  close(SPI->fd);
  free(SPI->msg);
  SPI->msg = NULL;
  free(SPI);
  SPI = NULL;
  printf("SPI cleaned up.\n");
  return 0;
}
