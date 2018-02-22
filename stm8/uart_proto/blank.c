#include <stm8s.h>
#define SP_ON_RESET 0x3FF
#include <soft_reset.h>
#include <common.h>
#include <serializer/serializer.h>
#include <tim4millis.h>
#include <uart.h>
#include <ring_buffer.h>
#include <string.h>
#include <uart_proto/proto.h>

struct {
	int a;
	int b;
	int c;
	int d;
} magic;


#define RX_BUFSIZE 16

uint16_t stackpointer = 0;

DEFINE_RING(rx, RX_BUFSIZE);
DEFINE_TIMER_8(active);

uint8_t trigger_reset;

#define te 	GPIOC->ODR &= ~(1 << 4)

PROTO_UART_INTERRUPT(rx);

int main() {
	trigger_reset = 0;
	CLK_INIT(CLOCK_SOURCE_INTERNAL, 0);
	uart_init(921600);
	pinmode(3, GPIOD, OUTPUT, OUTPUT_CR1_PUSHPULL, OUTPUT_CR2_10MHZ, 1);
	pinmode(2, GPIOD, OUTPUT, OUTPUT_CR1_PUSHPULL, OUTPUT_CR2_10MHZ, 1);
	pinmode(7, GPIOC, OUTPUT, OUTPUT_CR1_PUSHPULL, OUTPUT_CR2_10MHZ, 0);
	pinmode(6, GPIOC, OUTPUT, OUTPUT_CR1_PUSHPULL, OUTPUT_CR2_10MHZ, 1);
	pinmode(5, GPIOC, OUTPUT, OUTPUT_CR1_PUSHPULL, OUTPUT_CR2_10MHZ, 1);
	pinmode(4, GPIOC, OUTPUT, OUTPUT_CR1_PUSHPULL, OUTPUT_CR2_10MHZ, 1);
	pinmode(3, GPIOC, OUTPUT, OUTPUT_CR1_PUSHPULL, OUTPUT_CR2_10MHZ, 1);
	TIM4_init();
	proto_sharedaddr = &magic;




	_putchar = uart_putchar;
	_getchar = uart_getchar;

	__enable_interrupt();
	while(1){

		if(trigger_reset){
		soft_reset();
		}

		//__disable_interrupt();
		PROTO_HOOK(rx);

		if(TIMER_TICK_8(active, 100)){
			GPIOD->ODR ^= (1 << 2);
			TIMER_REFRESH_8(active);
		}



		//__enable_interrupt();
	}
}
