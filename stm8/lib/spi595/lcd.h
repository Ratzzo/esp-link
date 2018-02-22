#ifndef SPILCD_H
#define SPILCD_H
#include <stm8s.h>
#include <lcd/lcd_base.h>
#include "spi_595.h"

/*
void bitset_funct_t(lcd_t *lcd, uint8_t value, uint8_t mode)
{
    uint8_t byte = lcd->_master->status;
    byte &= ~(1 << value);
    byte |= (mode << value);
    spi595_setbyte(lcd->_master, byte);
}
*/

typedef struct spilcd_t{
    lcd_t lcd;
    spi595_t *master;
} spilcd_t;

void spilcd_create(spilcd_t *lcd, spi595_t *spi, uint8_t rs,  uint8_t enable,
                   uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);


#endif

