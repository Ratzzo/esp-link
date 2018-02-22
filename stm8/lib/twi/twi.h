#ifndef TWI_H
#define TWI_H

#include <stdint.h>
#include <stm8s.h>
#include <delay.h>
#include <common.h>

//interrupts not implemented yet
//void I2C_IRQHandler() __interrupt (19);

void twi_init();
void twi_setAddress(uint8_t);
uint8_t twi_readFrom(uint8_t, uint8_t*, uint8_t, uint8_t);
uint8_t twi_writeTo(uint8_t address, uint8_t* data, uint8_t length, uint8_t sendStop);
uint8_t twi_transmit(const uint8_t*, uint8_t);
void twi_attachSlaveRxEvent( void (*)(uint8_t*, int) );
void twi_attachSlaveTxEvent( void (*)(void) );
void twi_reply(uint8_t);
void twi_stop(void);
void twi_releaseBus(void);

#endif // TWI_H
