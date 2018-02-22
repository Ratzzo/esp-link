/*
Library to count milliseconds since start up, using Timer4 (8 bit) of STM8S MCUs

By ETPH 20160618, release under

Parts of code are copyrighted by STMicroelectronics licensed under MCD-ST Liberty SW License Agreement V2
http://www.st.com/software_license_agreement_liberty_v2

This library must be used with STM8S_StdPeriph_Lib package which is provided free of charge by STMicroelectronics

Brief guide:

- Two source files: stm8s_clk.c and stm8s_tim4.c must be added to project

- This code in tested on IAR. If using other compiler, the line "__IO uint32_t current_millis" might have to be put in the header file, and change to:
"volatile uint32_t current_millis"

- If your project is used with stm8s_it library, remember to comment out the function INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) in stm8s_it.c
*/

#include "tim4millis.h"

__IO uint32_t current_millis = 0x0; //--IO: volatile read/write
__IO uint8_t current_millis_uint8 = 0x0; //--IO: volatile read/write
__IO uint16_t current_millis_uint16 = 0x0; //--IO: volatile read/write

tim4_inthandler_t _tim4_inthandler;

//this entire function using ST's code
void TIM4_init(void)
{

        /* TIM4 configuration:
	- TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
	clock used is 16 MHz / 128 = 125 000 Hz
	- With 125 000 Hz we can generate time base:
	  max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
	  min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
	- In this example we need to generate a time base equal to 1 ms
	so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

	/* Time base configuration */
	//TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
        TIM4->PSCR = (uint8_t)(TIM4_PRESCALER_128);
        TIM4->ARR = (uint8_t)(124); //
        TIM4->SR1 = (uint8_t)(~TIM4_FLAG_UPDATE); //clear Update flag
        TIM4->IER |= TIM4_IER_UIE; //Update Interrupt Enable
        TIM4->CR1 |= TIM4_CR1_CEN; //enable TIM4
        _tim4_inthandler = 0;
			pinmode(2, GPIOA, OUTPUT, OUTPUT_CR1_PUSHPULL, OUTPUT_CR2_10MHZ, 0);
}


uint32_t millis(void)
{
	return current_millis;
}


//Interupt event, happen every 1 ms
void tim4_irq() __interrupt (IRQ_TIM4_UO)
{
	//increase 1, for millis() function
	current_millis++;
	current_millis_uint8++;
	current_millis_uint16++;
	if(_tim4_inthandler)
		_tim4_inthandler();

//	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
	TIM4->SR1 = (uint8_t)(~TIM4_IT_UPDATE);
	__no_operation();
}
