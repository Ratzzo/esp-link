#ifndef PCF8574_H
#define PCF8574_H

#include <stdint.h>
#include <twi.h>

#define PCF8574_FIXEDADDR    0b0100000

typedef struct pcf8574_t {
    volatile uint8_t address; //3 bit I2C addr. This is "OR"ed with 0x20
    volatile uint8_t raddress; //7 bit I2C addr.
    uint8_t state;
} pcf8574_t;

//uint8_t mcp23017_refresh(mcp23017_t *mcps); //get registers
//uint8_t mcp23017_update(mcp23017_t *mcps); //upload registers
uint8_t pcf8574_create(pcf8574_t *pcf, uint8_t addr);
uint8_t pcf8574_setbyte(pcf8574_t *pcf, uint8_t data);
int16_t pcf8574_getbyte(pcf8574_t *pcf);


#endif
