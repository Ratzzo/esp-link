#include "twi.h"

//i'm too lazy to implement frequency, however it shouldn't be that hard

void twi_init()
{


    I2C->CR1 &= ~I2C_CR1_PE;                     //  Diable I2C before configuration starts.
    //
    //  Setup the clock information.
    //
    I2C->FREQR = 16;                     //  Set the internal clock frequency (MHz).
    I2C->CCRH |= I2C_CCRH_FS;                   //  I2C running is standard mode.
    I2C->CCRL = 0xa0;                    //  SCL clock speed is 50 KHz.
    I2C->CCRH &= ~I2C_CCRH_CCR;
    //
    //  Set the address of this device.
    //
    I2C->OARH &= ~I2C_OARH_ADDMODE;               //  7 bit address mode.
    I2C->OARH |= I2C_OARH_ADDCONF;               //  Docs say this must always be 1.
    //
    //  Setup the bus characteristics.
    //
    I2C->TRISER = 17;
    //
    //  Turn off the interrupts.
    //
    I2C->ITR &= ~I2C_ITR_ITBUFEN;                //  Buffer interrupt
    I2C->ITR &= ~I2C_ITR_ITEVTEN;                //  Event interrupt
    I2C->ITR &= ~I2C_ITR_ITERREN;
    //
    //  Configuration complete so turn the peripheral on.
    //
    I2C->CR1 |=  I2C_CR1_PE;
    //
    //  Enter master mode.
    //
//       I2C->CR2 |= I2C_CR2_STOP;
}

volatile uint8_t twi_busy;
/*
uint8_t twi_readFrom(uint8_t address, uint8_t* data, uint8_t length, uint8_t sendStop)
{
    uint8_t len = length-1;
    uint8_t *dataptr = data;
    uint8_t count = 0;

    twi_busy = 1;
    //while(I2C->SR3 & I2C_SR3_BUSY){};
    I2C->CR2 |= I2C_CR2_START; // send START bit
    I2C->CR2 |= I2C_CR2_ACK;
    while (!(I2C->SR1 & I2C_SR1_SB)) {}; // wait for START condition (SB=1)

    I2C->DR = (address << 1) | 1;
    while(!(I2C->SR1 & I2C_SR1_ADDR)) {}; //wait for address transmission

    do {
        I2C->SR3;
 //       I2C->SR1;
        while (!(I2C->SR1 & I2C_SR1_RXNE)) {}; // wait for ByteReceived (RxNE=1)
        if(!len)
            I2C->CR2 &= ~I2C_CR2_ACK; //dont ack for last byte
        *dataptr++ = I2C->DR;
        count++;
    } while(len--);

//    I2C->SR3;
    if(sendStop){
        I2C->CR2 |= I2C_CR2_STOP; // send STOP bit
    while ((I2C->SR1 & I2C_SR1_SB)) {}; //wait for stop to take effect
    }

    twi_busy = 0;
    return count;
} */


//reimplemented according to datasheet, twi_writeto probably needs the same...
uint8_t twi_readFrom(uint8_t address, uint8_t* data, uint8_t length, uint8_t sendStop)
{
    uint8_t len = length;
    uint8_t ind = 0;
    while(I2C->SR3 & I2C_SR3_BUSY){};           //poll busy
    I2C->CR2 |= I2C_CR2_ACK;                    //set ack
    I2C->CR2 |= I2C_CR2_START;                  //set start
    while(!(I2C->SR1 & I2C_SR1_SB)){}           //poll CB
    I2C->DR = (address << 1) | 1;               //send with read bit
    while(!(I2C->SR1 & I2C_SR1_ADDR)) {};       //poll addr
    I2C->SR3;

    if(length >= 3){
        //fix this shit, see flowchart on i2c documentation for reading more than 3 bytes
        while(!(I2C->SR1 & I2C_SR1_BTF)) {};    //poll btf
        I2C->CR2 &= ~I2C_CR2_ACK;               //set ack
        //disable interrupts
        data[ind++] = I2C->DR;                  //read 1st byte
        if(sendStop)
            I2C->CR2 |= I2C_CR2_STOP;           //set stop
        data[ind++] = I2C->DR;                  //read 2nd byte
        //enable interrupts
        while(!(I2C->SR1 & I2C_SR1_RXNE)) {};   //poll rxne
        data[ind++] = I2C->DR;                  //read 3rd byte
    }
    else if(length == 2)
    {
         I2C->CR2 |= I2C_CR2_POS;               //set pos
        //disable interrupts
         I2C->SR1 &= ~I2C_SR1_ADDR;             //clear addr
         I2C->CR2 &= ~I2C_CR2_ACK;              //set ack
         //enable interrupts
        while(!(I2C->SR1 & I2C_SR1_BTF)) {};    //poll btf
        //disable interrupts
        if(sendStop)
             I2C->CR2 |= I2C_CR2_STOP;          //set stop
          data[0] = I2C->DR;                    //read 1st byte
          I2C->SR3;
          //enable interrupts
           data[1] = I2C->DR;                   //read 2nd byte

    }
    else
    {
        I2C->CR2 &= ~I2C_CR2_ACK;                //clear ack
    //    __disable_interrupt();                 //disable interrupts here
        I2C->CR1 &= ~I2C_SR1_ADDR;               //clear addr
        if(sendStop)
            I2C->CR2 |= I2C_CR2_STOP;            //set stop
    //    __enable_interrupt();                  //enable interrupts here
        while(!(I2C->SR1 & I2C_SR1_RXNE)) {};    //poll rxne
        data[0] = I2C->DR;
    }
    while(I2C->SR1 & I2C_SR1_STOPF) {};
    return 0;
}

uint8_t twi_writeTo(uint8_t address, uint8_t* data, uint8_t length, uint8_t sendStop){
    uint8_t len = length;
    uint8_t *dataptr = data;
    uint8_t count = 0;
    twi_busy = 1;
    // while(I2C->SR3 & I2C_SR3_BUSY){};
    //I2C->SR1 = I2C_SR1_SB;
    I2C->CR2 |= I2C_CR2_START; // send START bit
    while (!(I2C->SR1 & I2C_SR1_SB)) {}; // wait for START condition (SB=1)
    //GPIOA->ODR |= (1 << 3);
    I2C->DR = (address << 1); // slave address -> DR & write
    while (!(I2C->SR1 & I2C_SR1_ADDR)) {}; // wait for ADDRESS sent (ADDR=1)


    do{
        I2C->SR3; //clear SR3 flag
        while (!(I2C->SR1 & I2C_SR1_TXE)) {}; //wait for transmission of last byte
        I2C->DR = *dataptr++; //push data to register
        count++; //increment count
    } while(len--); //decrement len

    I2C->SR3;
    if(sendStop){
        I2C->CR2 |= I2C_CR2_STOP; // send STOP bit
    while ((I2C->SR1 & I2C_SR1_SB)) {}; //wait for stop to take effect
    }
    twi_busy = 0;
    return count;
}



/*
void I2C_IRQHandler() __interrupt (19)
{
        volatile unsigned char reg;
    if (I2C->SR1 & I2C_SR1_SB)
    {
        //
        //  Master mode, send the address of the peripheral we
        //  are talking to.  Reading SR1 clears the start condition.
        //
        reg = I2C->SR1;
        //
        //  Send the slave address and the read bit.
        //


       I2C->DR = (0b0100000 << 1) | 0; // 0 is write, 1 is read

      //  PB_ODR = 0x00;

        //
        //  Clear the address registers.
        //
       I2C->OARL &= ~I2C_OARL_ADD;
       I2C->OARH &= ~I2C_OARH_ADD;
        return;
    }
   if (I2C->SR1 & I2C_SR1_ADDR)
    {
        //
        //  In master mode, the address has been sent to the slave.
        //  Clear the status registers and wait for some data from the salve.
        //
        reg = I2C->SR1;
        reg = I2C->SR3;
        return;
    }
    if (I2C->SR1 & I2C_SR1_TXE)
    {

       I2C->DR = 0x00;
        reg =I2C->SR1;
        reg =I2C->SR3;
       I2C->CR2 |= I2C_CR2_STOP;
    }
    if (I2C->SR1 & I2C_SR1_RXNE)
    {
           I2C->CR2 |= I2C_CR2_STOP;
        return;
    }
    //
    //  If we get here then we have an error so clear
    //  the error and continue.
    //
    reg =I2C->SR1;
    reg =I2C->SR3;
    //
    //  Send a diagnostic signal to indicate we have cleared
    //  the error condition.
    //
//   GPIOD->ODR |= PD_ODR_ODR6;
    __no_operation();
 //  GPIOD->ODR &= ~PD_ODR_ODR6;
} */


//probably needs to set slave mode too
void twi_setAddress(uint8_t address)
{
    // set twi slave address (skip over TWGCE bit)
    I2C->OARL = address << 1;
}


