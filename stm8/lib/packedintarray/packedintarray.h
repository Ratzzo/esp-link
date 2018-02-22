#ifndef PACKEDINTARRAY_H
#define PACKEDINTARRAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define PACKEDINTARRAY_LENGTH_TYPE uint16_t
#define PACKEDINTARRAY_DATUM_TYPE uint32_t //this can be changed to accomodate for systems with a smaller stack

typedef struct packedintarray_t {
	uint8_t *data; //the actual data
	PACKEDINTARRAY_LENGTH_TYPE datasize; //datasize in bytes
	uint8_t intbits; //the number of bits per int
	PACKEDINTARRAY_LENGTH_TYPE length; //the lenght of the array
} packedintarray_t;

uint8_t packedintarray_create(packedintarray_t *p, uint8_t intbits, PACKEDINTARRAY_LENGTH_TYPE length);
PACKEDINTARRAY_DATUM_TYPE packedintarray_get(packedintarray_t *p, PACKEDINTARRAY_LENGTH_TYPE index);
uint8_t packedintarray_set(packedintarray_t *p, PACKEDINTARRAY_LENGTH_TYPE index, PACKEDINTARRAY_DATUM_TYPE datum);
uint8_t packedintarray_destroy(packedintarray_t *p);




#endif
