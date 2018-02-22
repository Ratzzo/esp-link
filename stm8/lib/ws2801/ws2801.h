/**
 *    ws2801 stm8 library
 *        Rtzz0 2017
 *      Public Domain
**/

#ifndef WS2801_H
#define WS2801_H

#include <stm8s.h>
#include <common.h>

//note1: the ws2801 doesn't have a CE pin, thus it always receives all the data from the SPI bus, however this could be fixed using external components.
//note2: the chip doesn't like the serial input provided by STM8S SPI module, so it doesn't provide a valid CKO, SDO. This could be fixed downgrading from hardware SPI to bitbang, however it would be way slower.

typedef struct ws2801_t {
    uint8_t r, g, b;
} ws2801_t;

void ws2801_create(ws2801_t *ws);
void ws2801_setdata(ws2801_t *ws, uint8_t r, uint8_t g, uint8_t b);

#endif
