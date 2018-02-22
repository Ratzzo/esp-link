#include "proto.h"

int proto_create(proto_t *proto, char *port){
	int result = 0;
	proto->state = 0;
	result = serial_create(&proto->iface, port, 921600);
	if(proto->iface.lasterror) return result;
	return result;
}

int proto_command(proto_t *proto, uint16_t com){
    uint16_t command = com;
	proto->buff[0] = *((uint8_t*)&command + 1);
	proto->buff[1] = *((uint8_t*)&command);
	proto->buff[2] = proto_crc8(0, proto->buff, 2);

	serial_write(&proto->iface, proto->buff, 3);
	if(proto->iface.lasterror) return __LINE__;

	serial_read(&proto->iface, proto->buff, 1);
	if(proto->iface.lasterror) return __LINE__;

	return 0;
}

//#define PROTO_DEBUGRETRY

//gets the shared address which is used as a reference
int proto_getsharedaddr(proto_t *proto, uint16_t *addr){
	uint8_t buff[20];
	#ifdef PROTO_DEBUGRETRY
	int retryc = 10;
	#endif
	buff[0] = proto_command(proto, PROTO_COMMAND_REQUESTSTRUCT);
	if(buff[0]) return __LINE__;

	buff[0] = 0x00; //ack back
	if(!serial_write(&proto->iface, buff, 1)) return __LINE__;

	retry:
	if(!serial_read(&proto->iface, buff, 3)) return __LINE__;
//	printf("calc crc8 = %x\n", proto_crc8(0, buff, sizeof(uint16_t)));
//	printf("got crc8 = %x\n", buff[2]);
	if(proto_crc8(0, buff, sizeof(uint16_t)) == buff[2])
	{
	#ifdef PROTO_DEBUGRETRY
	if(retryc == 0) {
	#endif
		((uint8_t*)addr)[0] = buff[1]; //Note: 16 bit big endian
		((uint8_t*)addr)[1] = buff[0]; //Note: 16 bit big endian
		buff[0] = PROTO_ACK;
	#ifdef PROTO_DEBUGRETRY
	}
	else
	{
		printf("debug retry\n");
		retryc--;
		buff[0] = PROTO_RETRY;
	}
	#endif
	}
	else
	{
		buff[0] = PROTO_RETRY;
	}

	if(!serial_write(&proto->iface, buff, 1)) return __LINE__;
	if(buff[0] == PROTO_RETRY) goto retry;

	return 0;
}


int proto_write(proto_t *proto, uint16_t addr, uint8_t *data, uint16_t size){
	uint8_t buff[20];
	int chunks = size/8;
	int left = size%8;
	int repeat;
	int retryn = 0;

	buff[0] = proto_command(proto, PROTO_COMMAND_WRITEMEM);
	//printf("buff[0] = %i\n", buff[0]);
	if(buff[0]) return __LINE__;

	buff[0] = *((uint8_t*)&addr+1);
	buff[1] = *((uint8_t*)&addr);
	buff[2] = *((uint8_t*)&size+1);
	buff[3] = *((uint8_t*)&size);
	buff[4] = proto_crc8(0, buff , 4);

	//printf("chunks %i\n", chunks);


	serial_write(&proto->iface, buff, 5);
	if(proto->iface.lasterror) return __LINE__;


	serial_read(&proto->iface, buff, 1);
	if(proto->iface.lasterror)	return __LINE__;

	if(buff[0]) return __LINE__;

	buff[0] = PROTO_ACK;
	serial_write(&proto->iface, buff, 1);
	if(proto->iface.lasterror) return __LINE__;
	for(repeat = 0; repeat < chunks; repeat++){
		//printf("repeat %i\n", repeat);

		memcpy(buff, data, 8);
		data += 8;
		buff[8] = proto_crc8(0, buff, 8);



		serial_write(&proto->iface, buff, 9);
		if(proto->iface.lasterror) return __LINE__;

		serial_read(&proto->iface, buff, 1); //2 for debug
		if(proto->iface.lasterror)	return __LINE__;

		if(buff[0] == PROTO_RETRY){
			data -= 8;
			repeat--;
			retryn++;
			if(retryn >= 100) break;
		//		printf("RETRY shit\n");
		}

		buff[0] = 0x00;
		serial_write(&proto->iface, buff, 1);
		if(proto->iface.lasterror) return __LINE__;

	}

	if(left > 0){
		memcpy(buff, data, left+1);
		buff[left] = proto_crc8(0, buff, left);

		serial_write(&proto->iface, buff, left+1);
		if(proto->iface.lasterror) return __LINE__;

		serial_read(&proto->iface, buff, 1);
		if(proto->iface.lasterror) return __LINE__;

//		printdata(buff, 6);

		buff[0] = 0;

		serial_write(&proto->iface, buff, 1);
		if(proto->iface.lasterror) return __LINE__;
	}
	return 0;
}

//returns 1: iface error (ie. no data received.)
//returns 2: protocol errors (ie. retry instead of ack, cable connected backwards)
int proto_read(proto_t *proto, uint16_t addr, uint8_t *data, uint16_t size){
	uint8_t buff[20];
	int chunks = size/8;
	int left = size%8;
	int repeat;

	buff[0] = proto_command(proto, PROTO_COMMAND_READMEM);

	if(buff[0]) return __LINE__;

	buff[0] = *((uint8_t*)&addr+1);
	buff[1] = *((uint8_t*)&addr);
	buff[2] = *((uint8_t*)&size+1);
	buff[3] = *((uint8_t*)&size);
	buff[4] = proto_crc8(0, buff , 4);

	serial_write(&proto->iface, buff, 5);
	if(proto->iface.lasterror) return __LINE__;


	serial_read(&proto->iface, buff, 1);
	if(proto->iface.lasterror)	return __LINE__;

	//printf("buff[0] = %x\n", buff[0]);
	if(buff[0]) return __LINE__;

	buff[0] = PROTO_ACK;
	serial_write(&proto->iface, buff, 1);
	if(proto->iface.lasterror) return __LINE__;
	for(repeat = 0; repeat < chunks; repeat++){
		//get buff chunk
//		serial_read(&proto->iface, buff, 1);
//		if(proto->iface.lasterror || buff[0] == PROTO_RETRY) return __LINE__;

		serial_read(&proto->iface, buff, 9);
		if(proto->iface.lasterror)	return __LINE__;

	//	printdata(buff, 9);
		uint8_t crc;




		//verify crc8
		if(buff[8] == (crc = proto_crc8(0, buff, 8))){
			memcpy(data, buff, 8);
			data += 8;
			buff[0] = PROTO_ACK;
			}
		else
		{
			data -= 8;
			repeat--;
			//printf("Retry!\n");
			buff[0] = PROTO_RETRY;
		}
		//printf("calculated checksum = %x\n", crc);


		serial_write(&proto->iface, buff, 1);
		if(proto->iface.lasterror) return __LINE__;
	}

	if(left > 0){
		serial_read(&proto->iface, buff, left+1);
		if(proto->iface.lasterror) return __LINE__;
		//verify crc8
		if(buff[left] == proto_crc8(0, buff, left)){
			memcpy(data, buff, left);
			buff[0] = PROTO_ACK;
			}
		else
		{
			buff[0] = PROTO_RETRY;
			//printf("Retry!\n");
		}

		serial_write(&proto->iface, buff, 1);
		if(proto->iface.lasterror) return __LINE__;
	}
	return 0;
}

int proto_destroy(proto_t *proto){
	return serial_destroy(&proto->iface);
}

uint8_t proto_crc8(uint8_t crc, uint8_t *data, uint8_t len)
{
   uint8_t i;
   while(len--){
//   crc += *data++;
   i = (*data++ ^ crc) & 0xff;
   crc = 0;
   if(i & (uint8_t)1)
	  crc ^= (uint8_t)0x5e;
   if(i & (uint8_t)2)
	  crc ^= (uint8_t)0xbc;
   if(i & (uint8_t)4)
	  crc ^= (uint8_t)0x61;
   if(i & (uint8_t)8)
	  crc ^= (uint8_t)0xc2;
   if(i & (uint8_t)0x10)
	  crc ^= (uint8_t)0x9d;
   if(i & (uint8_t)0x20)
	  crc ^= (uint8_t)0x23;
   if(i & (uint8_t)0x40)
	  crc ^= (uint8_t)0x46;
   if(i & (uint8_t)0x80)
	  crc ^= (uint8_t)0x8c;
	}
	return crc;
}

