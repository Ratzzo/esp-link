#include "7seg.h"
#include <twi.h>
#include <stdint.h>

//
//		6
//	5		7
//		4
//	3		1
//		2		0

uint8_t seg7_numbers[] = {
	(uint8_t)(1 << 2 | 1 << 3 | 1 << 1 | 1 << 5 | 1 << 7 | 1 << 6), 		 // 0
	(uint8_t)(1 << 1 | 1 << 7),												 // 1
	(uint8_t)(1 << 6 | 1 << 7 | 1 << 4 | 1 << 3 | 1 << 2),					 // 2
	(uint8_t)(1 << 6 | 1 << 7 | 1 << 4 | 1 << 1 | 1 << 2),					 // 3
	(uint8_t)(1 << 5 | 1 << 7 | 1 << 4 | 1 << 1),           			     // 4
	(uint8_t)(1 << 6 | 1 << 5 | 1 << 4 | 1 << 1 | 1 << 2), 					 // 5
	(uint8_t)(1 << 5 | 1 << 4 | 1 << 3 | 1 << 1 | 1 << 2), 					 // 6
	(uint8_t)(1 << 6 | 1 << 7 | 1 << 1), 									 // 7
	(uint8_t)(1 << 2 | 1 << 3 | 1 << 1 | 1 << 5 | 1 << 7 | 1 << 6 | 1 << 4), // 8
	(uint8_t)(1 << 5 | 1 << 6 | 1 << 7 | 1 << 4 | 1 << 1),					 // 9
	0 //blank
};

uint8_t seg7_create(seg7_t *seg, uint8_t digit0addr, uint8_t digit1addr){
    pcf8574_create(&seg->digit0, digit0addr);
 //   pcf8574_create(&seg->digit1, digit1addr);
    pcf8574_setbyte(&seg->digit0, 0xff);
//    pcf8574_setbyte(&seg->digit1, 0xff);
    return 0;
}

uint8_t seg7_setnumber(seg7_t *seg, int number){
    if(number < 0) number = 100 + (number % 100);
    if(number > 99) number %= 100;
	pcf8574_setbyte(&seg->digit0, ~((~seg->digit0.state & 1) | seg7_numbers[number % 10]));
//	pcf8574_setbyte(&seg->digit1, ~((~seg->digit1.state & 1) | seg7_numbers[number/10]));
	seg->currentnumber = number;
	return 0;
}

