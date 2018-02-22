#include "packedintarray.h"

uint8_t packedintarray_create(packedintarray_t *p, uint8_t intbits, PACKEDINTARRAY_LENGTH_TYPE length){
	unsigned int packsize = (intbits*length + 7)/8; //check how many truncated bytes that would be
	p->intbits = intbits;
	p->length = length;

	p->data = malloc(packsize);
	p->datasize = packsize;
	return 0;
}

uint32_t packedintarray_get(packedintarray_t *p, PACKEDINTARRAY_LENGTH_TYPE index){
	int bitsleft = p->intbits;
	int startingbit = (index*bitsleft)%8;
	int byte_number = (index*bitsleft)/8;
	int numberofbytes = (bitsleft+startingbit)/8;
	uint8_t currentmask;
	uint8_t byte;
	uint32_t out = 0;

	currentmask = 0xff << startingbit;

	while(numberofbytes--){
		byte = p->data[byte_number];
		out |= ((currentmask & byte) >> startingbit) << (p->intbits-bitsleft);
		bitsleft -= (8-startingbit);
		byte >>= (8-startingbit);
		startingbit = 0;
		currentmask = 0xff;
		byte_number++;
	}
		byte = p->data[byte_number];
		currentmask = 0xff >> (8 - bitsleft);
		out |= ((currentmask & byte) << (p->intbits-bitsleft));
//		printf("get ");
//		printbits(out);
		return out;

}

uint8_t packedintarray_set(packedintarray_t *p, PACKEDINTARRAY_LENGTH_TYPE index, PACKEDINTARRAY_DATUM_TYPE datum){
	int bitsleft = p->intbits;
	int startingbit = index*bitsleft%8;
	int byte_number = index*bitsleft/8;
	int numberofbytes = (bitsleft+startingbit)/8;
	uint8_t currentmask;
	uint8_t *byte = (uint8_t*)&p->data[byte_number];
//	printf("set ");
//	printbits(datum);

	currentmask = 0xff << startingbit;

	while(numberofbytes--){
		*byte &= ~currentmask;
		currentmask >>= startingbit;
		*byte |= ((currentmask & datum) << startingbit);
		bitsleft -= (8-startingbit);
		datum >>= (8-startingbit);
		currentmask = 0xff;
		startingbit = 0;
		byte++;
	}
	currentmask = 0xff >> (8 - bitsleft);
	*byte &= ~currentmask;
	*byte |= ((currentmask & datum));
	return 0;
}

uint8_t packedintarray_destroy(packedintarray_t *p){
	free(p->data);
	p->data = 0;
	return 0;
}
