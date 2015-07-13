#ifndef _SPI_H_
#define _SPI_H_

typedef struct
{
  int fd;
  uint8_t mode;
  uint8_t bpw;
  uint32_t msh;
} SPI_t;

int init_spi(void);
int spi_transfer(uint8_t tx[], uint8_t* rx);
int spi_cleanup(void);


#endif
