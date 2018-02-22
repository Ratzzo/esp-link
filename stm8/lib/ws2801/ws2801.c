#include "ws2801.h"

void ws2801_create(ws2801_t *ws){
    ws->r = 0;
    ws->g = 0;
    ws->b = 0;
}

void ws2801_setdata(ws2801_t *ws, uint8_t r, uint8_t g, uint8_t b){
    ws->r = r;
    ws->g = g;
    ws->b = b;
    spi_transfer(r);
    spi_transfer(g);
    spi_transfer(b);
}
