#include <esp8266.h>
#include <internals.h>
#include <uart.h>
#include <spi_flash.h>
#include <task.h>

#define LED (1 << 14)

extern int blinky_ram_user2(uint16_t led, int delay);
extern void memcpy_aligned(void * dst, const void * src, const unsigned int len);

uint32_t topkek = 19;

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

void ICACHE_FLASH_ATTR call_user_start(void){
	int stack;
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	gpio_output_set(1, 0, 1, 0);
	uart_init(CALC_UARTMODE(EIGHT_BITS, NONE_BITS, ONE_STOP_BIT),
		74880, 74880 ); //go 921600, because why not.

	uart0_muxed_printf(0, "PRINT1\n");
	uart0_muxed_printf(0, "PRINT2\n");
	uart0_muxed_printf(0, "PRINT3\n");
	uart0_muxed_printf(0, "PRINT4\n");
	uart0_muxed_printf(0, "PRINT5\n");
	uart0_muxed_printf(0, "PRINT6\n");
	uart0_muxed_printf(0, "PRINT7\n");
	uart0_muxed_printf(0, "PRINT8\n");
	uart0_muxed_printf(0, "PRINT9\n");
	uart0_muxed_printf(0, "PRINT10\n");
	uart0_muxed_printf(0, "PRINT11\n");
	uart0_muxed_printf(0, "PRINT12\n");
	uart0_muxed_printf(0, "PRINT13\n");
	uart0_muxed_printf(0, "PRINT14\n");
	uart0_muxed_printf(0, "PRINT15\n");
	uart0_muxed_printf(0, "PRINT16 %i\n", topkek);
	uart0_muxed_printf(0, "PRINT17 %x\n", blinky_ram_user2);

	if(topkek == 19)
	blinky_ram_user2(LED, 30000);
	else
	blinky_ram_user2(LED, 200000);
}






