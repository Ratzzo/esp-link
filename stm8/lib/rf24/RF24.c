#include "RF24.h"
#include "nRF24L01.h"
#include <delay.h>
#include <tim4millis.h>
#include <stm8s_spi.h>
#include <common.h>

#define _BV(n) (1 << n)
#define false 0
#define true 1
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#define LOW 0
#define HIGH 1
#define ce(x) rf24_ce(rf, x)



void rf24_ce(rf24_t *rf, int level)
{
    //digitalWrite(ce_pin,level);
    if(level)
        rf->ce_pin.port->ODR |= (1 << rf->ce_pin.pin);
    else
        rf->ce_pin.port->ODR &= ~(1 << rf->ce_pin.pin);

}

void rf24_csn(rf24_t *rf, int level)
{
    //digitalWrite(ce_pin,level);

    if(level)
        rf->csn_pin.port->ODR |= (1 << rf->csn_pin.pin);
    else
        rf->csn_pin.port->ODR &= ~(1 << rf->csn_pin.pin);

}

/*
uint8_t _spi_send_byte(uint8_t byte)
{
    SPI->DR = R_REGISTER | ( REGISTER_MASK & reg );
    while(!(SPI->SR & SPI_SR_TXE)) {};
    while(!(SPI->SR & SPI_SR_RXNE)) {};
    status = SPI->DR;
}

uint8_t _spi_recv_byte()
{
    SPI->DR = 0xff;
        while(!(SPI->SR & SPI_SR_TXE)) {};
        while(!(SPI->SR & SPI_SR_RXNE)) {};
    *buf++ = SPI->DR;
} */

uint8_t rf24_spi_send_command(rf24_t *rf, uint8_t command, uint8_t startcsn, uint8_t stopcsn){
    uint8_t result;
    if(startcsn)
        rf24_csn(rf, 0);
    result = spi_transfer(command);
    if(stopcsn)
        rf24_csn(rf, 1);
    return result;
}



uint8_t rf24_read_register(rf24_t *rf, uint8_t reg, uint8_t* buf, uint8_t len)
{
    uint8_t status = 0;

    rf24_csn(rf, 0);
    status = spi_transfer( R_REGISTER | ( REGISTER_MASK & reg ) );


    while ( len-- )
        *buf++ = spi_transfer(0xff);

    rf24_csn(rf, 1);

    return status;
}

uint8_t rf24_read_register2(rf24_t *rf, uint8_t reg)
{

    uint8_t buf;
    rf24_read_register(rf, reg, &buf, 1);
    return buf;

}

uint8_t rf24_write_register(rf24_t *rf, uint8_t reg, uint8_t* buf, uint8_t len)
{
    uint8_t status = 0;

    rf24_csn(rf, 0);

    status = spi_transfer(W_REGISTER | ( REGISTER_MASK & reg ));
    while ( len-- )
     spi_transfer(*buf++);

    rf24_csn(rf, 1);

    return status;
}


uint8_t rf24_write_register2(rf24_t *rf, uint8_t reg, uint8_t value)
{
    return rf24_write_register(rf, reg, &value, 1);
}

bool rf24_setDataRate(rf24_t *rf, uint8_t speed)
{
    uint8_t setup = 0;
    setup = rf24_read_register2(rf, RF_SETUP);

    // HIGH and LOW '00' is 1Mbs - our default
    rf->wide_band = false ;

    if(speed == RF_DR_2MBPS)
        rf->wide_band = true;

    rf24_write_register2(rf, RF_SETUP, speed);


    // Verify our result
    if (rf24_read_register2(rf, RF_SETUP) == speed)
    {
        return true;
    }
    else
    {
        rf->wide_band = false;
    }


    return false;
}


// TODO : get rid of these wasteful enums
uint8_t rf24_getPALevel(rf24_t *rf)
{
    return rf24_read_register2(rf, RF_SETUP) & (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
}

void rf24_setPALevel(rf24_t *rf, uint8_t level)
{
    rf24_write_register2(rf,  RF_SETUP, level) ;
}

uint8_t rf24_getDataRate(rf24_t *rf)
{
    return rf24_read_register2(rf, RF_SETUP) & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));
}

void rf24_setCRCLength(rf24_t *rf, rf24_crclength_e length)
{
    uint8_t config = rf24_read_register2(rf, CONFIG) & ~( _BV(CRCO) | _BV(EN_CRC)) ;
    // switch uses RAM (evil!)
    if ( length == RF24_CRC_DISABLED )
    {
        // Do nothing, we turned it off above.
    }
    else if ( length == RF24_CRC_8 )
    {
        config |= _BV(EN_CRC);
    }
    else
    {
        config |= _BV(EN_CRC);
        config |= _BV( CRCO );
    }
    rf24_write_register2(rf, CONFIG, config ) ;

}

/****************************************************************************/

rf24_crclength_e rf24_getCRCLength(rf24_t *rf)
{
    rf24_crclength_e result = RF24_CRC_DISABLED;
    uint8_t config = rf24_read_register2(rf, CONFIG) & ( _BV(CRCO) | _BV(EN_CRC)) ;

    if ( config & _BV(EN_CRC ) )
    {
        if ( config & _BV(CRCO) )
            result = RF24_CRC_16;
        else
            result = RF24_CRC_8;
    }

    return result;
}

void rf24_setChannel(rf24_t *rf, uint8_t channel)
{
    // TODO: This method could take advantage of the 'wide_band' calculation
    // done in setChannel() to require certain channel spacing.

    uint8_t max_channel = 127;
    rf24_write_register2(rf, RF_CH,min(channel,max_channel));
}



uint8_t rf24_get_status(rf24_t *rf)
{
    return rf24_spi_send_command(rf, NOP, 1, 1);
}

uint8_t rf24_create(rf24_t *rf, pp_t *_cepin, pp_t *_cspin)
{
    rf->ce_pin.port = _cepin->port;
    rf->ce_pin.pin = _cepin->pin;
    rf->csn_pin.port = _cspin->port;
    rf->csn_pin.pin = _cspin->pin;
    rf->wide_band = 1;
    rf->p_variant = 0;
    rf->payload_size = 32;
    rf->ack_payload_available = 0;
    rf->dynamic_payloads_enabled = 0;
    rf->pipe0_reading_address = 0;
    return 0;
}

uint8_t rf24_flush_rx(rf24_t *rf)
{
    return rf24_spi_send_command(rf, FLUSH_RX, 1, 1);
}

uint8_t rf24_flush_tx(rf24_t *rf)
{
    return rf24_spi_send_command(rf, FLUSH_TX, 1, 1);
}

uint8_t rf24_begin(rf24_t *rf)
{
    // Initialize pins
// pinMode(ce_pin,OUTPUT);
    rf->ce_pin.port->DDR |= (1 << rf->ce_pin.pin);
    rf->ce_pin.port->CR1 |= (1 << rf->ce_pin.pin);
    rf->ce_pin.port->CR2 |= (1 << rf->ce_pin.pin);
// pinMode(csn_pin,OUTPUT);
    rf->csn_pin.port->DDR |= (1 << rf->csn_pin.pin);
    rf->csn_pin.port->CR1 |= (1 << rf->csn_pin.pin);
    rf->csn_pin.port->CR2 |= (1 << rf->csn_pin.pin);
    // Initialize SPI bus
    ///SPI.begin();
//  SPI.setClockDivider(SPI_CLOCK_DIV8);

    rf24_ce(rf, 1);
    _delay_ms(5) ;
    rf24_ce(rf, 0);
    rf24_csn(rf, 1);

    // Must allow the radio time to settle else configuration bits will not necessarily stick.
    // This is actually only required following power up but some settling time also appears to
    // be required after resets too. For full coverage, we'll always assume the worst.
    // Enabling 16b CRC is by far the most obvious case if the wrong timing is used - or skipped.
    // Technically we require 4.5ms + 14us as a worst case. We'll just call it 5ms for good measure.
    // WARNING: Delay is based on P-variant whereby non-P *may* require different timing.
    _delay_ms( 5 ) ;

    // Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
    // WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
    // sizes must never be used. See documentation for a more complete explanation.
    rf24_write_register2(rf, SETUP_RETR,(0b0100 << ARD) | (0b1111 << ARC));

    // Restore our default PA level
    rf24_setPALevel(rf,  RF_PWR_0DB ) ;
    if(rf24_getPALevel(rf) != RF_PWR_0DB) return 1;

    // Determine if this is a p or non-p RF24 module and then
    // reset our data rate back to default value. This works
    // because a non-P variant won't allow the data rate to
    // be set to 250Kbps.
    if( rf24_setDataRate(rf, RF_DR_250KBPS ) )
    {
        rf->p_variant = true ;

    }

    // Then set the data rate to the slowest (and most reliable) speed supported by all
    // hardware.
    rf24_setDataRate(rf, RF_DR_1MBPS ) ;
    if(rf24_getDataRate(rf) != RF_DR_1MBPS) return 2;

    // Initialize CRC and request 2-byte (16bit) CRC
    rf24_setCRCLength(rf, RF24_CRC_16 ) ;
    if(rf24_getCRCLength(rf) != RF24_CRC_16) return 3;

    // Disable dynamic payloads, to match dynamic_payloads_enabled setting
    rf24_write_register2(rf, DYNPD,0);
    if(rf24_read_register2(rf, DYNPD) != 0) return 4;

    // Reset current status
    // Notice reset and flush is the last thing we do
    rf24_write_register2(rf, STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

    // Set up default configuration.  Callers can always change it later.
    // This channel should be universally safe and not bleed over into adjacent
    // spectrum.
    rf24_setChannel(rf, 76);

    rf24_flush_rx(rf);
    rf24_flush_tx(rf);
    return 0;
}

void rf24_PayloadSize(rf24_t *rf, uint8_t size)
{
    uint8_t max_payload_size = 32;
    rf->payload_size = min(size,max_payload_size);
}

/****************************************************************************/

uint8_t rf24_getPayloadSize(rf24_t *rf)
{
    return rf->payload_size;
}


void rf24_startListening(rf24_t *rf)
{
    rf24_write_register2(rf, CONFIG, rf24_read_register2(rf, CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX));
    rf24_write_register2(rf, STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

    // Restore the pipe0 adddress, if exists
    if (rf->pipe0_reading_address)
        rf24_write_register(rf, RX_ADDR_P0, (uint8_t*)rf->pipe0_reading_address, 5);

    // Flush buffers
    rf24_flush_rx(rf);
    rf24_flush_tx(rf);

    // Go!
    rf24_ce(rf, HIGH);

    // wait for the radio to come up (130us actually only needed)
    _delay_us(130);
}

void rf24_stopListening(rf24_t *rf)
{
    rf24_ce(rf, LOW);
    rf24_flush_tx(rf);
    rf24_flush_rx(rf);
}

void rf24_powerDown(rf24_t *rf)
{
    rf24_write_register2(rf, CONFIG, rf24_read_register2(rf, CONFIG) & ~_BV(PWR_UP));
}

void rf24_powerUp(rf24_t *rf)
{
    rf24_write_register2(rf, CONFIG,rf24_read_register2(rf, CONFIG) | _BV(PWR_UP));
}

uint8_t rf24_write_payload(rf24_t *rf, const void* buf, uint8_t len)
{
    uint8_t status = 0;
    const uint8_t* current = (uint8_t*)buf;
    volatile uint8_t data_len;
    volatile uint8_t blank_len;
    data_len = min(len,(rf->payload_size));
    blank_len = (rf->dynamic_payloads_enabled) ? 0 : rf->payload_size - data_len;

    //printf("[Writing %u bytes %u blanks]",data_len,blank_len);

    rf24_csn(rf, LOW);
    status = spi_transfer( W_TX_PAYLOAD );
    while ( data_len-- )
        spi_transfer(*current++);
    while ( blank_len-- )
        spi_transfer(0);
    rf24_csn(rf, HIGH);

    return status;
}

void rf24_startWrite(rf24_t *rf, const void* buf, uint8_t len )
{
    // Transmitter power-up
    rf24_write_register2(rf, CONFIG, ( rf24_read_register2(rf, CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
    _delay_us(150);

    // Send the payload
    rf24_write_payload(rf, buf, len );

    // Allons!
    rf24_ce(rf, HIGH);
    _delay_us(15);
    rf24_ce(rf, LOW);
}

void rf24_whatHappened(rf24_t *rf, bool *tx_ok, bool *tx_fail, bool *rx_ready)
{
    // Read the status & reset the status in one easy call
    // Or is that such a good idea?
    uint8_t status = rf24_write_register2(rf, STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

    // Report to the user what happened
    *tx_ok = status & _BV(TX_DS);
    *tx_fail = status & _BV(MAX_RT);
    *rx_ready = status & _BV(RX_DR);
}

void rf24_setRetries(rf24_t *rf, uint8_t delay, uint8_t count)
{
    rf24_write_register2(rf, SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}

void rf24_setPayloadSize(rf24_t *rf, uint8_t size)
{
    const uint8_t max_payload_size = 32;
    rf->payload_size = min(size,max_payload_size);
}

void rf24_openWritingPipe(rf24_t *rf, uint8_t *value)
{
    // Note that AVR 8-bit uC's store this LSB first, and the NRF24L01(+)
    // expects it LSB first too, so we're good.
    const uint8_t max_payload_size = 32;

    rf24_write_register(rf, RX_ADDR_P0, (uint8_t*)(value), 5);
    rf24_write_register(rf, TX_ADDR, (uint8_t*)(value), 5);
    rf24_write_register2(rf, RX_PW_P0,min(rf->payload_size,max_payload_size));
}

static const uint8_t child_pipe[] =
{
    RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
};
static const uint8_t child_payload_size[] =
{
    RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
};
static const uint8_t child_pipe_enable[] =
{
    ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5
};

void rf24_openReadingPipe(rf24_t *rf, uint8_t child, uint8_t *address)
{
    // If this is pipe 0, cache the address.  This is needed because
    // openWritingPipe() will overwrite the pipe 0 address, so
    // startListening() will have to restore it.
    if (child == 0)
        rf->pipe0_reading_address = address;

    if (child <= 6)
    {
        // For pipes 2-5, only write the LSB
        if ( child < 2 )
            rf24_write_register(rf, child_pipe[child], (uint8_t*)(address), 5);
        else
            rf24_write_register(rf, child_pipe[child], (uint8_t*)(address), 1);

        rf24_write_register2(rf, child_payload_size[child],rf->payload_size);

        // Note it would be more efficient to set all of the bits for all open
        // pipes at once.  However, I thought it would make the calling code
        // more simple to do it this way.
        rf24_write_register2(rf, EN_RXADDR,rf24_read_register2(rf, EN_RXADDR) | _BV(child_pipe_enable[child]));
    }
}

uint8_t rf24_getDynamicPayloadSize(rf24_t *rf)
{
    uint8_t result = 0;

    rf24_csn(rf, LOW);
    spi_transfer( R_RX_PL_WID );
    result = spi_transfer(0xff);
    rf24_csn(rf, HIGH);

    return result;
}

bool rf24_write(rf24_t *rf, const void* buf, uint8_t len )
{
    bool result = false;
    uint8_t observe_tx;
    uint8_t status;
    uint32_t sent_at;
    const uint32_t timeout = 500; //ms to wait for timeout
    bool tx_ok, tx_fail;

    // Begin the write
    rf24_startWrite(rf, buf,len);

    // ------------
    // At this point we could return from a non-blocking write, and then call
    // the rest after an interrupt

    // Instead, we are going to block here until we get TX_DS (transmission completed and ack'd)
    // or MAX_RT (maximum retries, transmission failed).  Also, we'll timeout in case the radio
    // is flaky and we get neither.

    // IN the end, the send should be blocking.  It comes back in 60ms worst case, or much faster
    // if I tighted up the retry logic.  (Default settings will be 1500us.
    // Monitor the send
    sent_at = millis();
    do
    {
        status = rf24_read_register(rf, OBSERVE_TX,&observe_tx,1);
        //IF_SERIAL_DEBUG(Serial.print(observe_tx,HEX));
    }
    while( ! ( status & ( _BV(TX_DS) | _BV(MAX_RT) ) ) && ( millis() - sent_at < timeout ) );

    // The part above is what you could recreate with your own interrupt handler,
    // and then call this when you got an interrupt
    // ------------

    // Call this when you get an interrupt
    // The status tells us three things
    // * The send was successful (TX_DS)
    // * The send failed, too many retries (MAX_RT)
    // * There is an ack packet waiting (RX_DR)
    rf24_whatHappened(rf, &tx_ok, &tx_fail, &rf->ack_payload_available);

    //printf("%u%u%u\r\n",tx_ok,tx_fail,ack_payload_available);

    result = tx_ok;
    //IF_SERIAL_DEBUG(Serial.print(result?"...OK.":"...Failed"));

    // Handle the ack packet
    if ( rf->ack_payload_available )
    {
        rf->ack_payload_length = rf24_getDynamicPayloadSize(rf);
//    IF_SERIAL_DEBUG(Serial.print("[AckPacket]/"));
//    IF_SERIAL_DEBUG(Serial.println(ack_payload_length,DEC));
    }

    // Yay, we are done.

    // Power down
    rf24_powerDown(rf);

    // Flush buffers (Is this a relic of past experimentation, and not needed anymore??)
    rf24_flush_tx(rf);

    return result;
}

uint8_t rf24_read_payload(rf24_t *rf, void* buf, uint8_t len)
{
    uint8_t status = 0;
    uint8_t* current = (uint8_t*)(buf);

    uint8_t data_len = min(len,rf->payload_size);
    uint8_t blank_len = rf->dynamic_payloads_enabled ? 0 : rf->payload_size - data_len;

    //printf("[Reading %u bytes %u blanks]",data_len,blank_len);

    rf24_csn(rf, LOW);
    status = spi_transfer( R_RX_PAYLOAD );
    //status = rf24_spi_send_command(rf, R_RX_PAYLOAD, 0, 0);
    while ( data_len-- )
        *current++ = spi_transfer(0xff);
    while ( blank_len-- )
        spi_transfer(0xff);
    rf24_csn(rf, HIGH);

    return status;
}


bool rf24_available(rf24_t *rf, uint8_t* pipe_num)
{
    uint8_t status = rf24_get_status(rf);

    // Too noisy, enable if you really want lots o data!!
    //IF_SERIAL_DEBUG(print_status(status));

    bool result = ( status & _BV(RX_DR) );

    if (result)
    {
        // If the caller wants the pipe number, include that
        if ( pipe_num )
            *pipe_num = (status >> RX_P_NO ) & 0b111;

        // Clear the status bit

        // ??? Should this REALLY be cleared now?  Or wait until we
        // actually READ the payload?

        rf24_write_register2(rf, STATUS,_BV(RX_DR) );

        // Handle ack payload receipt
        if ( status & _BV(TX_DS) )
        {
            rf24_write_register2(rf, STATUS,_BV(TX_DS));
        }
    }

    return result;
}

bool rf24_read(rf24_t *rf, void* buf, uint8_t len )
{
    // Fetch the payload
    rf24_read_payload(rf, buf, len );

    // was this the last of the data available?
    return (rf24_read_register2(rf, FIFO_STATUS) & _BV(RX_EMPTY));
}
