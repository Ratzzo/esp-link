#ifndef SPI_595
#define SPI_595

#include <stdint.h>
#include <common.h>

typedef struct spi595_t {
    uint8_t *latchport;
    uint8_t latchpin;
    uint8_t status;
} spi595_t;

uint8_t spi595_create(spi595_t *spi595, GPIO_TypeDef *latchport, uint8_t latchpin);
//write data and latch
uint8_t spi595_setbyte(spi595_t *spi595, uint8_t data);
//write data without latching
uint8_t spi595_writebyte(spi595_t *spi, uint8_t data);
//latch
uint8_t spi595_latch(spi595_t *spi);
int spi595_getbyte(spi595_t *spi595);


#endif // SPI_595
