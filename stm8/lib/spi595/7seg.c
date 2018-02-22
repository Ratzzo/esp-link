#include "7seg.h"

//
//   6
// 4   7
//   5
// 1   2
//   0   3

#define DIGIT(b0, b1, b2, b3, b4, b5, b6, b7)  ((uint8_t)(1 << b0) | (uint8_t)(1 << b1) | (uint8_t)(1 << b2) | (uint8_t)(1 << b3) | (uint8_t)(1 << b4) | (uint8_t)(1 << b5) | (uint8_t)(1 << b6) | (uint8_t)(1 << b7))

const uint8_t digits[] = {
	DIGIT(6, 4, 7, 2, 1, 0, 0, 0), //0
	DIGIT(7, 2, 2, 2, 2, 2, 2, 2), //1
	DIGIT(7, 6, 5, 1, 0, 0, 0, 0), //2
	DIGIT(6, 7, 5, 2, 0, 2, 2, 2), //3
	DIGIT(4, 5, 7, 2, 2, 2, 2, 2), //4
	DIGIT(6, 4, 5, 2, 0, 2, 2, 2), //5
	DIGIT(6, 4, 5, 2, 0, 1, 2, 2), //6
	DIGIT(7, 2, 6, 2, 2, 2, 2, 2), //7
	DIGIT(6, 4, 5, 7, 1, 0, 2, 2), //8
	DIGIT(6, 4, 5, 7, 0, 0, 2, 2), //9
	0
};

void spi595_7seg_setnumber(spi595_t *s, uint8_t number, uint8_t dot){
	spi595_setbyte(s, ~(digits[(number % 11)] | (dot ? 1 << 3 : 0)));
}


