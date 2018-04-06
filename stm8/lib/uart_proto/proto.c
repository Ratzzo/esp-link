#include "proto.h"


DEFINE_TIMER_8(proto_expire);

//state machine variables
void *proto_sharedaddr = 0xffff;
int proto_intparam;
uint8_t proto_uint8param;
uint8_t proto_crc;
uint8_t *proto_pointerparam;
uint8_t proto_buffparam[11];
uint16_t proto_uint16param;
uint16_t proto_command;
uint16_t proto_lastcommand;
uint8_t proto_state = 0;

void proto_expirecheck(){
	if(TIMER_TICK_8(proto_expire, 100)){
		proto_state = PROTO_STATE_IDLE;
		proto_uint8param = 0; //reset byte count
		TIMER_REFRESH_8(proto_expire);
	}
}

inline void proto_memcpy(uint8_t *dest, uint8_t *src, uint16_t size){
	while(size--){
		*dest++ = *src++;
	}
}

uint8_t proto_decode(uint8_t data){
	TIMER_REFRESH_8(proto_expire);
	//GPIOB->DDR |= (1 << 4);
	switch(proto_state){
	case PROTO_STATE_IDLE:
		proto_buffparam[proto_uint8param] = data;
		proto_uint8param++;
		if(proto_uint8param == 3)
			if(proto_buffparam[2] == proto_crc8(0, proto_buffparam, 2)){
				uart_putchar(PROTO_ACK);
				proto_command = *(uint16_t*)proto_buffparam;
				proto_state = PROTO_STATE_EXECUTING;
				proto_uint8param = 0;
				proto_uint16param = 0;
				proto_pointerparam = 0;
			}
			else
			{
				uart_putchar(PROTO_RETRY);
				proto_uint8param = 0;
			}
		break;
	case PROTO_STATE_EXECUTING:
		switch(proto_command){
		case PROTO_COMMAND_READMEM:
		case PROTO_COMMAND_WRITEMEM:
			if(proto_uint16param == 0){
				proto_lastcommand = proto_command;
				proto_intparam = 5; //2 bytes address, 2 bytes length, 1 byte proto_crc8
				proto_command = PROTO_COMMAND_IREADTOBUFF;
				proto_decode(data);
				break;
			}
			if(proto_crc8(0, proto_buffparam, 4) == proto_buffparam[4]){
				uart_putchar(PROTO_ACK);
			}
			else
			{
				proto_uint16param = 0;
				uart_putchar(PROTO_RETRY);
				break;
			}
			proto_command = proto_command == PROTO_COMMAND_READMEM ? PROTO_COMMAND_IREADPROCESSING : PROTO_COMMAND_IWRITEPROCESSING;
			proto_pointerparam = (uint8_t*)*(uint16_t*)(proto_buffparam); //address
			proto_uint16param = *(uint16_t*)(proto_buffparam + 2); //size
			proto_uint8param = proto_uint16param%8; //remainder
			proto_uint16param /= 8; //number of chunks
			proto_buffparam[9] = 0x00;
			break;
		case PROTO_COMMAND_IREADTOBUFF:
			proto_buffparam[proto_uint16param] = data;
			proto_uint16param++;
			if(proto_uint16param == proto_intparam){
				proto_command = proto_lastcommand;
				proto_decode(data);
			}
			break;
		case PROTO_COMMAND_REQUESTSTRUCT:
			//GPIOB->ODR &= (1 << 4);
			proto_command = PROTO_COMMAND_IREADPROCESSING;
			proto_pointerparam = (uint8_t*)(&proto_sharedaddr);
			proto_uint8param = sizeof(proto_sharedaddr);
			proto_uint16param = 0;
			proto_decode(data);
			break;
		case PROTO_COMMAND_IREADPROCESSING:
			if(proto_uint16param){ //process chunks
				proto_uint16param--;
				if(data == PROTO_RETRY){
					proto_pointerparam -= 8;
					proto_uint16param += 1;
				}
				//else we got PROTO_ACK
				proto_memcpy(proto_buffparam, proto_pointerparam, 8);
				for(proto_intparam = 0; proto_intparam < 8; proto_intparam++)
					uart_putchar(proto_buffparam[proto_intparam]);
				proto_crc = proto_crc8(0, proto_buffparam, 8);
				uart_putchar(proto_crc); //send proto_crc
				proto_pointerparam += 8;
				break;
			}
			//if there is data remaining
			if(proto_uint8param){
				if(proto_uint8param > 0x10){
					if(data == PROTO_RETRY)
						proto_uint8param -= 0x10;
					else {
						proto_uint8param = 0;
						proto_state = PROTO_STATE_IDLE;
						break;
					}
				}
				proto_memcpy(proto_buffparam, proto_pointerparam, proto_uint8param);
				for(proto_intparam = 0; proto_intparam < proto_uint8param; proto_intparam++)
					uart_putchar(proto_buffparam[proto_intparam]);
				//send proto_crc
				proto_crc = proto_crc8(0, proto_buffparam, proto_uint8param);
				uart_putchar(proto_crc);
				proto_uint8param += 0x10;
				break;
			}
			//if there is no chunks, and no remainder.
			if(proto_uint8param == 0 && proto_uint16param == 0){
				//uart_putchar(DONE);
				proto_state = PROTO_STATE_IDLE;
				proto_uint8param = 0;
			}
			break;
		case PROTO_COMMAND_IWRITEPROCESSING:
			//receive buffer
			if(proto_uint16param){ //process chunks

				//gather buffer, buff[0] is the PROTO_ACK
				if(proto_buffparam[10] < 9){
					proto_buffparam[proto_buffparam[10]] = data;
					proto_buffparam[10]++;
					break;
				}
				proto_buffparam[10] = 0;

				if(data == proto_crc8(0, proto_buffparam+1, 8)){
					proto_memcpy(proto_pointerparam, proto_buffparam+1, 8);
					proto_uint16param--;
					proto_pointerparam += 8;
					uart_putchar(PROTO_ACK);
				}
				else
					uart_putchar(PROTO_RETRY);
				break;
			}
			//if there is data remaining
			if(proto_uint8param){
				if(proto_buffparam[10] < proto_uint8param+1){
					proto_buffparam[proto_buffparam[10]] = data;
					proto_buffparam[10]++;
					break;
				}
				proto_buffparam[10] = 0;
				if(data == proto_crc8(0, proto_buffparam+1, proto_uint8param)){
					proto_memcpy(proto_pointerparam, proto_buffparam+1, proto_uint8param);
					proto_uint8param = 0;
					uart_putchar(PROTO_ACK);
				}
				else
					uart_putchar(PROTO_RETRY);
				break;
			}
			//if there is no chunks, and no remainder.
			if(proto_uint8param == 0 && proto_uint16param == 0){
				//uart_putchar(DONE);
				proto_state = PROTO_STATE_IDLE;
				proto_uint8param = 0;
			}
			break;
		}

	}

	return 0;
}

uint8_t proto_crc8(uint8_t proto_crc, uint8_t *data, uint8_t len)
{
   uint8_t i;
   while(len--){
//   proto_crc += *data++;
   i = (*data++ ^ proto_crc) & 0xff;
   proto_crc = 0;
   if(i & (uint8_t)1)
	  proto_crc ^= (uint8_t)0x5e;
   if(i & (uint8_t)2)
	  proto_crc ^= (uint8_t)0xbc;
   if(i & (uint8_t)4)
	  proto_crc ^= (uint8_t)0x61;
   if(i & (uint8_t)8)
	  proto_crc ^= (uint8_t)0xc2;
   if(i & (uint8_t)0x10)
	  proto_crc ^= (uint8_t)0x9d;
   if(i & (uint8_t)0x20)
	  proto_crc ^= (uint8_t)0x23;
   if(i & (uint8_t)0x40)
	  proto_crc ^= (uint8_t)0x46;
   if(i & (uint8_t)0x80)
	  proto_crc ^= (uint8_t)0x8c;
	}
	return proto_crc;
}

