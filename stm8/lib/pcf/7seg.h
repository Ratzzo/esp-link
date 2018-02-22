#ifndef SEG7_H
#define SEG7_H

#include <stdint.h>
#include <twi.h>
#include <pcf8574.h>
//set the display to show a number from 0 to 99


typedef struct seg7_t {
    pcf8574_t digit0;
    pcf8574_t digit1;
    uint8_t currentnumber;
} seg7_t;

uint8_t seg7_create(seg7_t *seg, uint8_t digit0addr, uint8_t digit1addr);

//set a number from 0 to 99
uint8_t seg7_setnumber(seg7_t *seg, int number);


#endif
