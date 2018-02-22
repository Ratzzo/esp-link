#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <stm8s.h>

uint8_t uart_init(uint32_t speed);
uint8_t uart_printf(char *format, ...);
int uart_putchar(int ch);
char uart_getchar(void);

#endif
