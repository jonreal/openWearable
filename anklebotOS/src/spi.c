#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "common.h"
#include "spi.h"

SPI_t SPI;

int init_spi(void)
{

  int fd;
  SPI.bpw = 8;
  SPI.msh = 500000;

  if(load_device_tree("BB-SPIDEV0") != 1){
    printf("Unable to load spi device tree.\n");
    return -1;
  }

  fd = open("/dev/spidev1.0", O_RDWR);
  if(fd < 0){
    printf("cannot open spi device.\n");
    return -1;
  }
  SPI.fd = fd;

  // mode
  if(ioctl(fd, SPI_IOC_WR_MODE, &SPI.mode) == -1){
    printf("can't set spi mode\n");
    return -1;
  }
  if(ioctl(fd, SPI_IOC_RD_MODE, &SPI.mode) == -1){
    printf("can't get spi mode\n");
    return -1;
  }

  // bits per word
  if(ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &SPI.bpw) == -1){
    printf("can't set bits per word\n");
    return -1;
  }
  if(ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &SPI.bpw) == -1){
    printf("can't get bits per word\n");
    return -1;
  }

  // max speed in Hz
  if(ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &SPI.msh) == -1){
    printf("can't set max speed\n");
    return -1;
  }
  if(ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &SPI.msh) == -1){
    printf("can't get max speed\n");
    return -1;
  }

  printf("spi mode: %d\n", SPI.mode);
	printf("bits per word: %d\n", SPI.bpw);
	printf("max speed: %d Hz (%d KHz)\n", SPI.msh, SPI.msh/1000);

  return 0;
}

int spi_transfer(uint8_t tx[], uint8_t* rx)
{
  int ret;

  struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = 0,
		.speed_hz = SPI.msh,
		.bits_per_word = SPI.bpw,
	};

  ret = ioctl(SPI.fd, SPI_IOC_MESSAGE(1), &tr);
  if(ret < 1){
    printf("can't send spi message\n");
    printf("ret %i\n", ret);
    return -1;
  }

  /*
  for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("%.2X ", rx[ret]);
	}
	puts("");
  */

  return 0;
}

int spi_cleanup(void)
{
  close(SPI.fd);
  printf("spi cleaned up.\n");

  return 0;
};

