#include <esp8266.h>
#include <internals.h>
#include <uart.h>

extern void Cache_Read_Disable(void);
extern uint32 SPIRead(uint32, void*, uint32);
extern void Cache_Read_Enable(uint32, uint32, uint32);

romheader_t _user2;

#define SPIRead(a, b, c) Cache_Read_Disable(); SPIRead(a, b, c); Cache_Read_Enable(0,0,0)

void user_init(void){
		uint32_t addr;
		romheader_t user2;
		segmentheader_t seg;
		addr = (uint32_t)APP_USER2_BASE_VADDR;
		addr -= 0x40200000;
		//get first rom header
		SPIRead(addr, &_user2, sizeof(_user2));
		addr += sizeof(_user2);
		addr += _user2.size;
		//get second rom header (this contains iram)
		SPIRead(addr, &user2, sizeof(user2));
		addr += sizeof(user2);
		//copy iram to it's address
		SPIRead(addr, (void*)user2.map_address, user2.size);
		addr += user2.size;


		//what comes next is the segments(literals, initialization values)
		int i;
		uint32_t nskip = 0;
		for(i=0; i < user2.n_segments; i++){
			addr += nskip;
			SPIRead(addr, &seg, sizeof(seg));
			addr += sizeof(seg);
			if(seg.offset){
				SPIRead(addr, (void*)seg.offset, seg.size);
			}
			nskip = seg.size;
		}
	(*(void(*)())_user2.entry_point)();
}
