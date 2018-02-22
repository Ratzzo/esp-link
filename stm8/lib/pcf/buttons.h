#ifndef PCFBUTTONS_H
#define PCFBUTTONS_H

#include <stm8s.h>
#include <stdint.h>
#include <tim4millis.h>
#include "pcf8574.h"
#include <common.h>

typedef uint8_t (*pcfbuttons_handler_t)(uint8_t state, uint8_t button);

typedef struct pcfbuttons_t {
	pcf8574_t *master;
	uint8_t interrupt_pin_mask;
	GPIO_TypeDef *interrupt_port;
	uint8_t buttons;
	uint8_t prevbuttons;
	uint8_t pending;
	uint8_t buttonmask;
	uint8_t lastmillis;
	pcfbuttons_handler_t handler;
} pcfbuttons_t;


int pcfbuttons_create(pcf8574_t *master, uint8_t buttonsmask, pcfbuttons_handler_t handler, uint8_t interrupt_pin, GPIO_TypeDef *interrupt_port);
void pcfbuttons_handle_interrupt();
int pcfbuttons_listen();
#endif
