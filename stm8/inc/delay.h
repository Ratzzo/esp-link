/*
 * delay utilite for STM8 family
 * COSMIC and SDCC
 * Terentiev Oleg
 * t.oleg@ymail.com
 */

#ifndef _UTIL_DELAY_H_
#define _UTIL_DELAY_H_ 1

void _delay_cycl( unsigned short __ticks );
void _delay_us( uint32_t __us );
void _delay_ms( uint32_t __ms );

#endif

