#ifndef PROTO_H
#define PROTO_H

#ifdef PROTO_SERVER
#include <stm8s.h>
#include <common.h>
#include <stdint.h>
#include <uart.h>
#include <tim4millis.h>
#endif

#define PROTO_STATE_IDLE				0
#define PROTO_STATE_DECODINGCOMMAND		1
#define PROTO_STATE_VERYFYING			2
#define PROTO_STATE_EXECUTING			3

#define PROTO_COMMAND_READMEM			0x11
#define PROTO_COMMAND_WRITEMEM			0x12
#define PROTO_COMMAND_IREADTOBUFF		0x43
#define PROTO_COMMAND_IREADPROCESSING	0x44
#define PROTO_COMMAND_IWRITEPROCESSING	0x45
#define PROTO_COMMAND_IWRITEACK			0x46
#define PROTO_COMMAND_IREADACK			0x47
#define PROTO_COMMAND_REQUESTSTRUCT		0x48

#define PROTO_ACK		0x00
#define PROTO_RETRY		0x01 //bad crc
#define PROTO_DONE		0x03

#ifdef PROTO_SERVER

extern void *proto_sharedaddr;

#define PROTO_UART_INTERRUPT(ring)	void uart_charhandler() __interrupt(IRQ_UART1_RDF) {	\
										__disable_interrupt();					\
										RING_PUT(rx, uart_getchar());			\
										__enable_interrupt();					\
									} struct proto_uart_interrupt_s


#define PROTO_HOOK(ring) 		while(RING_NOTEMPTY(ring)){			\
									uint8_t current_char;			\
									RING_GET(ring, current_char);	\
									proto_decode(current_char);		\
								}									\
								proto_expirecheck()


uint8_t proto_crc8(uint8_t crc, uint8_t *data, uint8_t len);
uint8_t proto_decode(uint8_t data);
void proto_expirecheck();
#endif


#endif
