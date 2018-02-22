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


#ifndef TIM4MILLIS_H_
#define TIM4MILLIS_H_
#include <common.h>
#include "stm8s.h"

#define DEFINE_TIMER_8(name) uint8_t name##_millis = 0
#define DEFINE_TIMER_16(name) uint16_t name##_millis = 0
#define DEFINE_TIMER(name) uint32_t name##_millis = 0

#define TIMER_TICK_8(name, interval) ((uint8_t)(current_millis_uint8 - name##_millis) > interval)
#define TIMER_TICK_16(name, interval) ((uint16_t)(current_millis_uint16 - name##_millis) > interval)
#define TIMER_TICK(name, interval) ((uint32_t)(current_millis - name##_millis) > interval)

#define TIMER_REFRESH_8(name) (name##_millis = current_millis_uint8)
#define TIMER_REFRESH_16(name) (name##_millis = current_millis_uint16)
#define TIMER_REFRESH(name) (name##_millis = current_millis)

#define TIM4_PERIOD 255
//volatile uint32_t current_millis
extern __IO uint32_t current_millis;
extern __IO uint8_t current_millis_uint8;
extern __IO uint16_t current_millis_uint16;

typedef void(*tim4_inthandler_t)();
extern tim4_inthandler_t _tim4_inthandler;

void TIM4_init(void);

uint32_t millis(void);

void tim4_irq() __interrupt (IRQ_TIM4_UO);

#endif
