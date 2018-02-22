#ifndef PROTO_CLIENT_H
#define PROTO_CLIENT_H

#include <serial.h>
#include <string.h>
#include <stdio.h>
#include "../stm8/lib/uart_proto/proto.h"

typedef struct proto_t {
	serial_t iface;
	uint8_t state;
	uint8_t buff[8];
} proto_t;

int proto_create(proto_t *proto, char *port);
int proto_command(proto_t *proto, uint16_t command);
int proto_getsharedaddr(proto_t *proto, uint16_t *addr);
int proto_write(proto_t *proto, uint16_t addr, uint8_t *data, uint16_t size);
int proto_read(proto_t *proto, uint16_t addr, uint8_t *data, uint16_t size);
int proto_destroy(proto_t *proto);
uint8_t proto_crc8(uint8_t crc, uint8_t *data, uint8_t len);

#endif
