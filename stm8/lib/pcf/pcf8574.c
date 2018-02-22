#include "pcf8574.h"
#include <twi.h>
#include <stdint.h>

uint8_t pcf8574_create(pcf8574_t *pcf, uint8_t addr)
{
	int16_t readByte;
	pcf->address = addr;
	pcf->raddress = PCF8574_FIXEDADDR | addr;

	if((readByte = pcf8574_getbyte(pcf)) == -1) return 1;
	if(pcf8574_setbyte(pcf, (uint8_t)readByte)) return 2;
	return 0;
}

uint8_t pcf8574_setbyte(pcf8574_t *pcf, uint8_t data)
{
    pcf->state = data;
        if(twi_writeTo(pcf->raddress, &data, 1, 1)) return 1; //send and stop
	return 0;
}

int16_t pcf8574_getbyte(pcf8574_t *pcf)
{
		uint8_t byte[10];
	//if(i2c_master_writeTo(mcps->master, mcps->raddress, bytes, 1, 0, 10)) return -1; //send and reset device reach timeout
		twi_readFrom(pcf->raddress, byte, 1, 1);
		return byte[0];
}



