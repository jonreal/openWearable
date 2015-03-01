#ifndef _SPI_H_
#define _SPI_H_

typedef struct{
  int fd;
  int mode;
  struct spi_ioc_transfer *msg;
} SPI_t;

int init_spi(void);
int spi_transfer(uint8_t tx[], uint8_t* rx);
int spi_cleanup(void);

#endif
