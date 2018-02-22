#include <uart.h>

int uart_putchar(int ch){
    UART1->DR = ch;
    while(!(UART1->SR & UART1_SR_TXE)){};
    return ch;
}

char uart_getchar(void){
    uint8_t data = 0;

//    while(!(UART1->SR & UART1_SR_TXE)){};
    while(!(UART1->SR & UART1_SR_RXNE)){};
    data = UART1->DR;
    return data;
}

uint8_t uart_init(uint32_t speed){
        //
        //  Clear the Idle Line Detected bit in the status register by a read
        //  to the UART1_SR register followed by a Read to the UART1_DR register.
        //
        uint8_t v = UART1->SR;
        v = UART1->DR;

        v = F_CPU/speed;
        //
        //  Reset the UART registers to the reset values.
        //
        UART1->CR1 = 0;
/*        UART1->CR2 = 0;
        UART1->CR3 = 0;
        UART1->CR4 = 0;
        UART1->CR5 = 0;*/

        //
        //  Now setup the port to speed,n,8,1.
        //
        UART1->CR1 &= ~(UART1_CR1_M | UART1_CR1_PCEN);        //  8 Data bits and disable parity
        UART1->CR3 &= ~(UART1_CR3_STOP);     //  1 stop bit.
        UART1->BRR2 = v & 0xf;
        UART1->BRR1 = (v & 0xf0) >> 4;
        //
        //  Disable the transmitter and receiver.
        //
        UART1->CR2 &= ~(UART1_CR2_TEN | UART1_CR2_REN);
        //
        //  Set the clock polarity, lock phase and last bit clock pulse.
        //
        UART1->CR3 |= UART1_CR3_CPOL | UART1_CR3_CPHA | UART1_CR3_LBCL;
        //
        //  Turn on the UART transmit, receive and the UART clock.
        //
        UART1->CR2  |= UART1_CR2_TEN | UART1_CR2_REN | UART1_CR2_RIEN;
//        UART1->CR3  |= UART1_CR3_CKEN; //we are not using synchronous uart, moron.
        return 0;
}

uint8_t uart_printf(char *format, ...){
    char buffer[32];
    uint8_t written;
    uint8_t *b = buffer;
    va_list args;
    va_start (args, format);
    written = vsprintf(buffer, format, args);
    va_end (args);
    while(written--){
        UART1->DR = *b++;
        while(!(UART1->SR & UART1_SR_TXE)){};
    }
    return written;
}
