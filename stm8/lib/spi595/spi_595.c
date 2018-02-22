#include "spi_595.h"
#include <stdint.h>
#include <common.h>


uint8_t spi595_create(spi595_t *spi595, GPIO_TypeDef *latchport, uint8_t latchpin)
{
    uint8_t lp = (1 << latchpin);
    spi595->latchport = &latchport->ODR;
    spi595->latchpin = lp;
    //configure latch
    pinmode(latchpin, latchport, OUTPUT, OUTPUT_CR1_PUSHPULL, OUTPUT_CR2_10MHZ, LOW);
/*    latchport->DDR |=  lp; //out
    latchport->CR1 |=  lp; //pushpull
    latchport->CR2 |=  lp; //10mhz
    latchport->ODR &= ~lp; */
	return 0;
}

uint8_t spi595_setbyte(spi595_t *spi595, uint8_t data)
{
    spi_transfer(data);
	spi595_latch(spi595);
    spi595->status = data;
	return 0;
}

uint8_t spi595_writebyte(spi595_t *spi, uint8_t data){
	spi_transfer(data);
	spi->status = data;
	return 0;
}

uint8_t spi595_latch(spi595_t *spi595){
	*spi595->latchport |= spi595->latchpin; //latch data
	*spi595->latchport &= ~spi595->latchpin; //terminate latching
	return 0;
}

int spi595_getbyte(spi595_t *spi595)
{
	return spi595->status;
}



