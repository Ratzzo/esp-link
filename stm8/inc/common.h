#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <stm8s.h>

typedef int (*putchar_f)(int);
typedef char (*getchar_f)();

extern getchar_f _getchar;
extern putchar_f _putchar;

#define IRQ_TLI         0  /** External top level interrupt */
#define IRQ_AWU         1  /** Auto wake up from halt */
#define IRQ_CLK         2  /** Clock controller */
#define IRQ_EXTI0       3  /** Port A external interrupts */
#define IRQ_EXTI1       4  /** Port B external interrupts */
#define IRQ_EXTI2       5  /** Port C external interrupts */
#define IRQ_EXTI3       6  /** Port D external interrupts */
#define IRQ_EXTI4       7  /** Port E external interrupts */
#define IRQ_SPI         10 /** End of transfer */
#define IRQ_TIM1_UOUTB  11 /** TIM1 update/overflow/underflow/trigger/break */
#define IRQ_TIM1_CC     12 /** TIM1 capture/compare */
#define IRQ_TIM2_UO     13 /** TIM2 update/overflow */
#define IRQ_TIM2_CC     14 /** TIM2 capture/compare */
#define IRQ_TIM3_UO     15 /** TIM3 update/overflow */
#define IRQ_TIM3_CC     16 /** TIM4 capture/compare */
#define IRQ_UART1_TXC   17 /** Tx complete */
#define IRQ_UART1_RDF   18 /** Receive register DATA FULL */
#define IRQ_I2C         19 /** I2C interrupt */
#define IRQ_UART2_TXC   20 /** Tx complete */
#define IRQ_UART2_RDF   21 /** Receive register DATA FULL */
#define IRQ_ADC         22 /** ADC1 end of conversion/analog watchdog interrupt */
#define IRQ_TIM4_UO     23 /** TIM4 update/overflow */
#define IRQ_FLASH       24 /**  EOP/WR_PG_DIS */

typedef enum GPIO_DDR_t{
   PINMODE_INPUT=0,
   PINMODE_OUTPUT=1
} GPIO_DDR_t;

typedef enum GPIO_CR1_t{
    PINMODE_INPUT_CR1_FLOATING=0,
    PINMODE_INPUT_CR1_PULLUP=1,
    PINMODE_OUTPUT_CR1_OPENDRAIN=0,
    PINMODE_OUTPUT_CR1_PUSHPULL=1
} GPIO_CR1_t;

typedef enum GPIO_CR2_t{
    PINMODE_INPUT_CR2_NOINTERRUPT=0,
    PINMODE_INPUT_CR2_INTERRUPT=1,
    PINMODE_OUTPUT_CR2_2MHZ=0,
    PINMODE_OUTPUT_CR2_10MHZ=1
} GPIO_CR2_t;

#define INPUT   0
#define OUTPUT  1

#define LOW     0
#define HIGH    1

#define FALLING 0
#define RISING  1

#define INPUT_CR1_FLOATING 0
#define INPUT_CR1_PULLUP   1


#define OUTPUT_CR1_OPENDRAIN   0
#define OUTPUT_CR1_PUSHPULL    1

#define INPUT_CR2_NOINTERRUPT  0
#define INPUT_CR2_INTERRUPT    1

#define OUTPUT_CR2_2MHZ     0
#define OUTPUT_CR2_10MHZ    1


/** ENABLE INDEPENDENT WATCHDOG
IWDG->KR = 0xCC; //enable independent watchdog, these values are constants defined in the stm8 reference manual.
IWDG->KR = 0x55; //allow register writing
IWDG->PR = IWDG_Prescaler_256;
IWDG->RLR = 0xff;
IWDG->KR = 0xAA; //restart timer
*/


#define IWDG_INIT(prescaler, rlr) {                         \
                                    IWDG->KR = 0xCC;        \
                                    IWDG->KR = 0x55;        \
                                    IWDG->PR = prescaler;   \
                                    IWDG->RLR = rlr;        \
                                    IWDG->KR = 0xAA;        \
                                   }

#define IWDG_FEED() IWDG->KR = 0xAA


/** CONFIGURE SYSTEM CLOCK
CLK->ICKR = 0;                       //  Reset the Internal Clock Register.
CLK->ICKR |= CLK_ICKR_HSIEN;                 //  Enable the HSI.
CLK->ECKR = 0;                       //  Disable the external clock.
while (!(CLK->ICKR & CLK_ICKR_HSIRDY));       //  Wait for the HSI to be ready for use.
CLK->CKDIVR = 0;                     //  Ensure the clocks are running at full speed.
CLK->PCKENR1 = 0xff;                 //  Enable all peripheral clocks.
CLK->PCKENR2 = 0xff;                 //  Ditto.
CLK->CCOR = 0;                       //  Turn off CCO.
CLK->HSITRIMR = 0;                   //  Turn off any HSIU trimming.
CLK->SWIMCCR = 0;                    //  Set SWIM to run at clock / 2.
CLK->SWR = 0xe1;                     //  Use HSI as the clock source.
CLK->SWCR = 0;                       //  Reset the clock switch control register.
CLK->SWCR |= CLK_SWCR_SWEN;                //  Enable switching.
while (CLK->SWCR & CLK_SWCR_SWBSY);        //  Pause while the clock switch is busy.
*/

#define CLOCK_SOURCE_INTERNAL CLK->ICKR
#define CLOCK_SOURCE_EXTERNAL CLK->ECKR

#define CLK_INIT(CLOCK_SOURCE, DIVIDER) {                                         \
                    CLOCK_SOURCE |= CLK_ICKR_HSIEN;                               \
                    while (!(CLOCK_SOURCE & CLK_ICKR_HSIRDY));                    \
                    CLK->CKDIVR = 0;                                              \
                    CLK->PCKENR1 = 0xff;                                          \
                    CLK->PCKENR2 = 0xff;                                          \
                    CLK->CCOR = 0;                                                \
                    CLK->HSITRIMR = 0;                                            \
                    CLK->SWIMCCR = 0;                                             \
                    CLK->SWR = 0xe1;                                              \
                    CLK->SWCR = 0;                                                \
                    CLK->SWCR |= CLK_SWCR_SWEN;                                   \
                    while (CLK->SWCR & CLK_SWCR_SWBSY);                           \
                    }


/**  // Enable SPI Clock.
CLK->PCKENR1 |= (1 << (CLK_PERIPHERAL_SPI & 0x0F));

//Set the MOSI, MISO and SCk as pushpull.
GPIOC->CR1 |= (GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5);

// SPI Configuration
SPI->CR1 |= SPI_FIRSTBIT_MSB
            | SPI_BAUDRATEPRESCALER_2
            | SPI_CLOCKPOLARITY_LOW
            | SPI_CLOCKPHASE_1EDGE
            | SPI_MODE_MASTER; //spi master mode


SPI->CR2 |= SPI_DATADIRECTION_2LINES_FULLDUPLEX //data direction
            | SPI_NSS_HARD //slave management
            | SPI_CR2_SSI; //spi master mode

SPI->CRCPR = 0x07; //crc polynomial

//Enable SPI.
SPI->CR1 |= SPI_CR1_SPE;
 */

#define SPI_INIT_MASTER(prescaler, pol, phase){                                                     \
                    CLK->PCKENR1 |= (1 << (CLK_PERIPHERAL_SPI & 0x0F));                             \
                                                                                                    \
                                                                                                    \
                    GPIOC->CR1 |= (GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5);                               \
                                                                                                    \
                    SPI->CR1 |= SPI_FIRSTBIT_MSB                                                    \
                                | prescaler                                                         \
                                | pol                                                               \
                                | phase                                                             \
                                | SPI_MODE_MASTER;                                                  \
                                                                                                    \
                                                                                                    \
                    SPI->CR2 |= SPI_DATADIRECTION_2LINES_FULLDUPLEX                                 \
                                | SPI_NSS_HARD                                                      \
                                | SPI_CR2_SSI;                                                      \
                                                                                                    \
                    SPI->CRCPR = 0x07;                                                              \
                                                                                                    \
                    SPI->CR1 |= SPI_CR1_SPE;                                                        \
                    }


typedef struct pp_t {
    GPIO_TypeDef *port;
    uint8_t pin;
} pp_t;


pp_t *pinport(GPIO_TypeDef *port, uint8_t pin);

uint8_t pinmode(uint8_t pin, GPIO_TypeDef *port, GPIO_DDR_t ddr, GPIO_CR1_t cr1, GPIO_CR2_t cr2, uint8_t odr);

/*!
 * \brief spi_transfer
 * \param byte
 * \return
 */
uint8_t spi_transfer(uint8_t byte);

#endif // _COMMON_H_
