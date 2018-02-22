#include "common.h"

pp_t _pp_circular_buffer[15];
uint8_t _pp_buffer_index = 0;


getchar_f _getchar;
putchar_f _putchar;

char getchar(){
    return _getchar();
}

int putchar(int ch){
    return _putchar(ch);
}


uint8_t pinmode(uint8_t pin, GPIO_TypeDef *port, GPIO_DDR_t ddr, GPIO_CR1_t cr1, GPIO_CR2_t cr2, uint8_t odr){

    uint8_t retval = !!(port->IDR & (1 << pin));
    uint8_t mask = (1 << pin);

    port->DDR = (port->DDR & ~mask) | (ddr << pin);
    port->CR1 = (port->CR1 & ~mask) | (cr1 << pin);
    port->CR2 = (port->CR2 & ~mask) | (cr2 << pin);
    port->ODR = (port->ODR & ~mask) | (odr << pin);

    return retval;

}

pp_t *pinport(GPIO_TypeDef *port, uint8_t pin)
{
    pp_t *p = &_pp_circular_buffer[_pp_buffer_index % 15];
    p->port = port;
    p->pin = pin;
    _pp_buffer_index++;
    return p;
}
//blocking spi transfer
uint8_t spi_transfer(uint8_t byte)
{
    SPI->DR = byte;

    //wait for transmission
    while(!(SPI->SR & SPI_SR_RXNE)) {};
    while(!(SPI->SR & SPI_SR_TXE)) {};

    //read the register back
    return SPI->DR;
}

#ifndef F_CPU
//#warning F_CPU is not defined!
#define F_CPU 16000000
#endif


/*
 * Func delayed N cycles, where N = 3 + ( ticks * 3 )
 * so, ticks = ( N - 3 ) / 3, minimum delay is 6 CLK
 * when tick = 1, because 0 equels 65535
 */

void _delay_cycl( unsigned short __ticks )
{
#if defined(__CSMC__)
/* COSMIC */
  #define T_COUNT(x) (( F_CPU * x / 1000000UL )-3)/3)
        // ldw X, __ticks ; insert automaticaly
        _asm("nop\n $N:\n decw X\n jrne $L\n nop\n ", __ticks);
#elif defined(__SDCC)
  #define T_COUNT(x) (( F_CPU * x / 1000000UL )-5)/5)
        __asm__("nop\n nop\n");
        do { 		// ASM: ldw X, #tick; lab$: decw X; tnzw X; jrne lab$
                __ticks--;//      2c;                 1c;     2c    ; 1/2c
        } while ( __ticks );
        __asm__("nop\n");
#elif defined(__RCST7__)
/* RAISONANCE */
  #error ToDo for RAISONANCE
#elif defined(__ICCSTM8__)
/* IAR */
  #error ToDo for IAR
#else
 #error Unsupported Compiler!          /* Compiler defines not found */
#endif
}

void _delay_us( uint32_t __us )
{
        _delay_cycl( (uint32_t)( T_COUNT(__us) );
}

void _delay_ms( uint32_t __ms )
{
        while ( __ms-- )
        {
                _delay_us( 1000 );
        }
}



