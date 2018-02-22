#include "serial.h"

//#define DEBUG__

#ifdef DEBUG__
#include <stdio.h>
extern uint8_t printdata(char *data, uint32_t size);
#endif

char *_serial_error_strings[] = {
	"Success.",
	"Unable to open serial port.",
	"Unable to write to serial port.",
	"Unable to read from port.",
	"Read timed out.",
	"",
};

char *serial_geterrorstring(serial_t *s){
	uint8_t lasterror = s->lasterror;
	s->lasterror = 0;
	return _serial_error_strings[lasterror];
}


#define PROTO_READ_TIMEOUT_USEC 10000

#ifdef __unix__

uint32_t serial_decodebaud(uint32_t baud){
	switch(baud){
	case 1200:
	return B1200;
	case 1800:
	return B1800;
	case 2400:
	return B2400;
	case 4800:
	return B4800;
	case 9600:
	return B9600;
	case 19200:
	return B19200;
	case 38400:
	return B38400;
	case 57600:
	return B57600;
	case 115200:
	return B115200;
	case 230400:
	return B230400;
	case 460800:
	return B460800;
	case 500000:
	return B500000;
	case 576000:
	return B576000;
	case 921600:
	return B921600;
	}
	return 0;
}

int serial_create(serial_t *s, const char *com, uint32_t baud){
	s->lasterror = 0;
	s->fd = open(com, O_RDWR | O_NOCTTY);
	if(s->fd == -1) {
		s->lasterror = 1;
		return 1;
	}
	FD_ZERO(&s->fdset);
	FD_SET(s->fd, &s->fdset);

	s->timeout.tv_sec = 0;
	s->timeout.tv_usec = PROTO_READ_TIMEOUT_USEC;

	tcgetattr(s->fd, &s->opt);
	s->opt.c_cflag = serial_decodebaud(baud) | CS8;
	s->opt.c_iflag &= ~IGNBRK;         // disable break processing
	s->opt.c_lflag = 0;                // no signaling chars, no echo,
	s->opt.c_oflag = 0;                // no remapping, no delays
	s->opt.c_cc[VMIN]  = -1;
	s->opt.c_cc[VTIME] = 1;
	//s->opt.c_iflag &= ~(IXON | IXOFF | IXANY);
	s->opt.c_iflag = 0;
	s->opt.c_cflag |= (CLOCAL | CREAD);
	s->opt.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	s->opt.c_cflag &= ~CSTOPB;
	s->opt.c_cflag &= ~CRTSCTS;
	tcsetattr(s->fd, TCSANOW, &s->opt);

	return 0;
}



int serial_write(serial_t *s, const void *buf, size_t n){
	int result = write(s->fd, buf, n);
	#ifdef DEBUG__
	printf("+write ");
	printdata(buf, n);
	printf("%i\n", result);
	#endif
	if(result == -1){
		s->lasterror = 2;
		return 0;
	}
	s->lasterror = 0;
	return result;
}

int serial_read(serial_t *s, void *buf, size_t n){
	int result = 0;
	s->lasterror = 0;
	if(select(s->fd + 1, &s->fdset, NULL, NULL, &s->timeout) <= 0){
		s->lasterror = 4;
		return 0;
	}
	s->timeout.tv_usec = PROTO_READ_TIMEOUT_USEC;
	s->lasterror = 0;
	result = read(s->fd, buf, n);
	#ifdef DEBUG__
	printf("+read ");
	printdata(buf, n);
	printf("%i\n", result);
	#endif
	return result;
}

int serial_destroy(serial_t *s){
	close(s->fd);
	return 0;
}
#elif defined(__WIN32__)
//I'm lazy, refactor may be needed
uint32_t serial_decodebaud(uint32_t baud){
return baud;
}

int serial_create(serial_t *s, const char *com, uint32_t baud){
	DCB conf = {0};
	COMMTIMEOUTS timout = {0};
	s->lasterror = 0;
	s->fd = CreateFile(com, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if(s->fd == INVALID_HANDLE_VALUE) {
		s->lasterror = 1;
		return __LINE__;
	}

	if(GetCommState(s->fd, &conf))
{
    conf.BaudRate = 921600;
    conf.ByteSize = 8;
    conf.Parity = NOPARITY;
    conf.StopBits = ONESTOPBIT;
    conf.fBinary = TRUE;
    conf.fParity = FALSE;
	conf.fDtrControl = 0x00;
	conf.fOutX = 0;
	conf.fInX = 0;
	conf.fNull = 0;

}
else
{
	s->lasterror = 1;
	serial_destroy(s);
	return __LINE__;
}

if(!SetCommState(s->fd, &conf)){
		s->lasterror = 1;
		serial_destroy(s);
		return __LINE__;
}

if(GetCommTimeouts(s->fd, &timout)){
	timout.ReadIntervalTimeout     = PROTO_READ_TIMEOUT_USEC;
    timout.ReadTotalTimeoutConstant     = PROTO_READ_TIMEOUT_USEC;
    timout.ReadTotalTimeoutMultiplier     = PROTO_READ_TIMEOUT_USEC;
    timout.WriteTotalTimeoutConstant     = PROTO_READ_TIMEOUT_USEC;
    timout.WriteTotalTimeoutMultiplier = PROTO_READ_TIMEOUT_USEC;
    memcpy(&s->timeout, &timout, sizeof(timout));
}
else
{
		s->lasterror = 1;
		serial_destroy(s);
		return __LINE__;
}

if(!SetCommTimeouts(s->fd, &timout)){
		s->lasterror = 1;
		serial_destroy(s);
		return __LINE__;
}

	return 0;

}

int serial_write(serial_t *s, const void *buf, size_t n){
	uint32_t written = 0;
	s->lasterror = 0;
    if(!WriteFile(s->fd, buf, n, (PDWORD)&written, NULL))
	{
//		printf("erro %x\n", GetLastError());
		s->lasterror = 2;
		return -1;
	}
	printf("req %i written %i\n", n, written);
	printdata(buf, n);
    return written;
}

int serial_read(serial_t *s, void *buf, size_t n){
	int result = 0;
	COMMTIMEOUTS timout;
	memcpy(&timout, &s->timeout, sizeof(timout));
	SetCommTimeouts(s->fd, &timout);
	if(!ReadFile(s->fd, buf, n, (PDWORD)&result, NULL))
	{
		//printf("erro %x\n", GetLastError());
		s->lasterror = 2;
		return -1;
	}
	printf("req %i read %i\n", n, result);
	printdata(buf, n);
	return result;
}


int serial_destroy(serial_t *s){
	CloseHandle(s->fd);
	return 0;
}
#elif defined(__ESP8266__)

int serial_create(serial_t *s, const char *file, uint32_t baud){
	s->lasterror = 0;
	s->mux = (uint8_t)file;
	return 0;
}
int serial_write(serial_t *s, const void *buf, size_t n){

}
int serial_read(serial_t *s, void *buf, size_t n){

}

int serial_destroy(serial_t *s){

}
#else
#error "not implemented yet for this operating system, but you can always take the initiative."
#endif

