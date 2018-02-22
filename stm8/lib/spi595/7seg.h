#ifndef D5957SEG_H
#define D5957SEG_H
#include <stm8s.h>
#include "spi_595.h"

void spi595_7seg_setnumber(spi595_t *s, uint8_t number, uint8_t dot);

#endif
