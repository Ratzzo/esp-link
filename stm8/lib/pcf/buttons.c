#include "buttons.h"

volatile pcfbuttons_t __pcf;

int pcfbuttons_create(pcf8574_t *master, uint8_t buttonsmask, pcfbuttons_handler_t handler, uint8_t interrupt_pin, GPIO_TypeDef *interrupt_port){
	//set interrupt
	pinmode(interrupt_pin, interrupt_port, INPUT, INPUT_CR1_PULLUP, INPUT_CR2_INTERRUPT, HIGH);
	pcf8574_setbyte(master, 0xff);
	__pcf.interrupt_pin_mask = 1 << interrupt_pin;
	__pcf.interrupt_port = interrupt_port;
	__pcf.prevbuttons = buttonsmask;
	__pcf.buttonmask = buttonsmask;
	__pcf.pending = 0;
	__pcf.master = master;
	__pcf.handler = handler;
	return 0;
}

void pcfbuttons_handle_interrupt(){
	__disable_interrupt();
	__pcf.interrupt_port->CR2 &= ~__pcf.interrupt_pin_mask;
	if(!__pcf.pending){
		__pcf.pending = 1;
		__pcf.lastmillis = current_millis_uint8;
	}

	__enable_interrupt();
}

int pcfbuttons_listen(){
	__disable_interrupt();

		if(__pcf.pending && (uint8_t)(current_millis_uint8 - __pcf.lastmillis) > 20){
			uint8_t i;
			__pcf.buttons = pcf8574_getbyte(__pcf.master) & __pcf.buttonmask;
			if(__pcf.handler)
				if(__pcf.buttons != __pcf.prevbuttons){
					for(i=0; i < 8; i++)
						if(((~__pcf.prevbuttons ^ ~__pcf.buttons) >> i) & 1){
								__pcf.handler(__pcf.buttons >> i & 1, i);
						}
					__pcf.prevbuttons = __pcf.buttons & __pcf.buttonmask;
				}

			__pcf.interrupt_port->CR2 |= __pcf.interrupt_pin_mask;
			__pcf.pending = 0;
		}
	__enable_interrupt();
	return 0;
}
