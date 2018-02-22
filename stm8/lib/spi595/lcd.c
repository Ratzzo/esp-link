#include "lcd.h"

void spi_lcd_bitset_func(spilcd_t *lcd, uint8_t value, uint8_t mode)
{
    uint8_t byte = lcd->master->status;
    byte &= ~(1 << value);
    byte |= (mode << value);
    spi595_setbyte(lcd->master, byte);
}

void spilcd_create(spilcd_t *lcd, spi595_t *spi, uint8_t rs,  uint8_t enable,
                   uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
    lcd->master = spi;
    lcd_create(&lcd->lcd, (lcd_bitset_funct_t)spi_lcd_bitset_func, rs, enable, d0, d1,d2,d3);
}
