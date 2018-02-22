#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/internals.h"

#define minaddr 0x40100000 //see esp8266 memory map

#define abort(message)	{ \
	printf(message); \
	exit(1); \
	}
	
#define textmap(array, index) array[index % (sizeof(array)/sizeof(char*))]
	
#pragma pack(push, 1)

char *flash_interfaces[] = {"QIO",
			    "QOUT",
			    "DIO",
			    "DOUT"};
			    

char *flash_sizes[] = 	   {"512k",
			    "256k",
			    "1M",
			    "2M",
			    "4M"};
	
char *flash_freqs[] = 	   {"40Mhz",
			    "26Mhz",
			    "20Mhz",
			    " ",
			    " ",
			    " ",
			    " ",
			    " ",
			    " ",
			    " ",
			    " ",
			    " ",
			    " ",
			    " ",
			    " ",
			    "80Mhz"};

#pragma pack(pop)

FILE *fp;
			    
//dump all literals found with their addresses+baseaddr.
int dumpliterals(uint32_t baseaddr, char *mem, int size){
	int max_lit_size = 100; //max literal size
	char string[100];
	uint8_t valid_char_min = 0x20;
	uint8_t valid_char_max = 0xF7;
	int pos = 0;
	int stringpos;
	printf("scanning for string literals...\n");
	while(pos < size){
		unsigned char chr;
		stringpos = pos;

		//start of valid string
		do {
			chr = mem[stringpos];
			if(chr == 0xa)  mem[stringpos] = ' ';
			if(chr == 0xd)  mem[stringpos] = ' ';
			stringpos++;
			if(!chr){
				printf("%x %s\n", baseaddr+pos, mem+pos);
				break;
			}
		} while((chr >= valid_char_min && chr <= valid_char_max) || chr == 0xd || chr == 0xa || chr == 0);


		pos=stringpos;
	}
	printf("\n");
	return 1;
}
			    
int main(int argc, char *argv[]){
	uint8_t lit = 0; //dump literals option
	romheader_t romh;
	uint32_t pos = 0;
	char *line = 0;
	size_t len = 0;
	ssize_t read;
	if(argc < 2) abort("no file specified\n");
	fp = fopen(argv[1], "r");
	if(!fp) abort("cannot open file specified\n");
	if(argc >= 3 && (argv[2])[0] == 'l') lit = 1; 
	
	for(int i = 0; i < 2; i++){
	fread(&romh, 1, sizeof(romh), fp);
	pos += sizeof(romh);
	printf("rom header:\n");
	printf("magic\t\t\t0x%X\n", 					       romh.magic);
	printf("n_segments\t\t0x%X\n",                                    romh.n_segments);
	printf("flash_interface\t\t%s\n", textmap(flash_interfaces, romh.flash_interface));
	printf("flash_size\t\t%s\n",                textmap(flash_sizes, romh.flash_size));
	printf("flash_freq\t\t%s\n",                textmap(flash_freqs, romh.flash_freq));
	printf("entry_point\t\t0x%x\n",                                romh.entry_point);
	printf("mapped into\t\t0x%x\n",                                romh.map_address);
	printf("size\t\t\t0x%x\n\n",									romh.size);
	
	pos += romh.size;
	fseek(fp, pos, SEEK_SET);
	}
	int nskip = 0;
	for(int i = 0; i < romh.n_segments-1; i++){
	segmentheader_t seg;
	pos += nskip;
	fseek(fp, pos, SEEK_SET);
	printf("segment:\n", i); 
	fread(&seg, 1, sizeof(seg), fp);
	printf("offset\t\t0x%x\n", seg.offset);
	printf("size\t\t0x%x\n",   seg.size);
	//printf("addr\t\t0x%x\n",   seg.addr);
	printf("pos\t\t%x\n", pos);
//	printf("file pos = %u\n", ftell(fp));
	pos += sizeof(seg);
	//handle literal dumping
	if(lit){
		char *data = malloc(seg.size);
		if(data) {
			fread(data, 1, seg.size, fp);
			dumpliterals(seg.offset, data, seg.size);
		}
		free(data);
	}

	nskip = seg.size;
	}
	
	
	return 0;
}
