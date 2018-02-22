#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define minaddr 0x40100000 //see esp8266 memory map

#define abort(message)	{ \
	printf(message); \
	exit(1); \
	}

FILE *fp;

int main(int argc, char *argv[]){
	char *line = 0;
	size_t len = 0;
	ssize_t read;
	if(argc < 2) abort("no file specified\n");
	fp = fopen(argv[1], "r");
	if(!fp) abort("cannot open file specified\n");
	
	int i = 0;
	int addr;
	char flag[100]; //l(local), g(global), or w
	char section[100]; //section name
	int alsize; //alignment or size
	char name[100];

	while((read = getline(&line, &len, fp)) != -1 && i < 10){
		int ret = sscanf(line, "%x %7c %s %x %s", &addr, flag, section, &alsize, name);
		if(ret > 0 && addr > minaddr &&
		strstr(name, ".irom0") == 0 &&
		strstr(name, "call_user_start") == 0 &&
		strstr(name, "irom0") == 0)
		printf("PROVIDE(%s = 0x%x);\n", name, addr);
		
		//i++;
	}
	
	
	return 0;
}
