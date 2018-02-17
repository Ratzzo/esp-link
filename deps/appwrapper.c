#include <esp8266.h>
#include <internals.h>

#include <uart.h>


romheader_t _user2;


void ICACHE_FLASH_ATTR user_init(void){
	uint32_t addr;
	romheader_t user2;
	segmentheader_t seg;


	addr = (uint32_t)APP_USER2_BASE_VADDR; 
	//get first rom header
	ets_memcpy(&_user2, (const void*)addr, sizeof(_user2));
	addr += sizeof(_user2);
	addr += _user2.size;
	//get second rom header (this contains iram)
	ets_memcpy(&user2, (const void*)addr, sizeof(user2));
	addr += sizeof(user2);
	//copy iram to it's address
	ets_memcpy((void*)user2.map_address, (const void*)addr, user2.size);
	addr += user2.size;
	
		
	
	//what comes next is the segments (dram, dport... I think)
	int i;
	uint32_t nskip = 0;
	for(i=0; i < user2.n_segments; i++){
		addr += nskip;
		ets_memcpy(&seg, (const void*)addr, sizeof(seg));
		addr += sizeof(seg);
		if(seg.offset){
			ets_memcpy((void*)seg.offset, (void*)(addr), seg.size);
		}
		nskip = seg.size;
	}
	
	(*(void(*)())_user2.entry_point)();
}

int blinky(uint16_t led, int delay){
	while(1){
	  os_delay_us(delay);
	  gpio_output_set(led, 0, led, 0);
	  os_delay_us(delay);
	  gpio_output_set(0, led, led, 0);
	  system_soft_wdt_feed();
	}
	return 0;
}
