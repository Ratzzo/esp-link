#include <esp8266.h>
#include <internals.h>
#include <uart.h>

#define LED (1 << 14)

extern int blinky_ram_user2(uint16_t led, int delay);

int topkek = 19;

void ICACHE_FLASH_ATTR call_user_start(void){
	uart_init(CALC_UARTMODE(EIGHT_BITS, NONE_BITS, ONE_STOP_BIT),
		74880, 74880); //go 921600, because why not.
	
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	
	//blinky_ram_user2(LED, 40000);

	gpio_output_set(1, 0, 1, 0);
/*	uart0_write_char('M');
	uart0_write_char('O');
	uart0_write_char('T');
	uart0_write_char('H');
	uart0_write_char('E');
	uart0_write_char('R');
	uart0_write_char('F');
	uart0_write_char('U');*/
	uart0_muxed_printf(0, "MOTHERFUCKER1\n");
	uart0_muxed_printf(0, "MOTHERFUCKER1\n");
//	blinky(LED, 30000);
	
	if(topkek == 19)
	blinky_ram_user2(LED, 40000);
	else
	blinky_ram_user2(LED, 200000);
}


int blinky_ram_user2(uint16_t led, int delay){
	while(1){
	  os_delay_us(delay);
	  gpio_output_set(led, 0, led, 0);
	  os_delay_us(delay);
	  gpio_output_set(0, led, led, 0);
	  system_soft_wdt_feed();
	}
	return 0;
}




