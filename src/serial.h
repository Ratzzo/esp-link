#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#ifdef __unix__
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct serial_t {
	struct termios opt;
	int fd;
	fd_set fdset;
	struct timeval timeout;
	uint8_t lasterror;
} serial_t;
#elif defined(__WIN32__)
#include <windows.h>
typedef struct serial_t {
	HANDLE fd;
	uint8_t lasterror;
	COMMTIMEOUTS timeout;
} serial_t;
#elif defined(__ESP8266__)
#include <esp8266.h>
#include <uart.h>
typedef struct serial_t {
	uint8_t mux; //either uart0, or uart1
	uint8_t lasterror;
} serial_t;
#else
typedef struct serial_t {
	//implement me
} serial_t;
#error "please implement serial prototypes for your operating system"
#endif

int serial_create(serial_t *s, const char *file, uint32_t baud);
int serial_write(serial_t *s, const void *buf, size_t n);
int serial_read(serial_t *s, void *buf, size_t n);
int serial_destroy(serial_t *s);
char *serial_geterrorstring(serial_t *s);

#endif
