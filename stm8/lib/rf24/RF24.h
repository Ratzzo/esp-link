#ifndef RF24_H
#define RF24_H

#include <stdint.h>
#include <stm8s.h>
#include <common.h>

/**
 * CRC Length.  How big (if any) of a CRC is included.
 *
 * For use with setCRCLength()
 */
typedef enum { RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16 } rf24_crclength_e;

typedef struct rf24_t {
  pp_t ce_pin; /**< "Chip Enable" pin, activates the RX or TX role */
  pp_t csn_pin; /**< SPI Chip select */
  bool wide_band; /* 2Mbs data rate in use? */
  bool p_variant; /* False for RF24L01 and true for RF24L01P */
  uint8_t payload_size; /**< Fixed size of payloads */
  bool ack_payload_available; /**< Whether there is an ack payload waiting */
  bool dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */
  uint8_t ack_payload_length; /**< Dynamic size of pending ack payload. */
  uint8_t *pipe0_reading_address; /**< Last address set on pipe 0 for reading. */
} rf24_t;

uint8_t rf24_read_register(rf24_t *rf, uint8_t reg, uint8_t* buf, uint8_t len);
uint8_t rf24_read_register2(rf24_t *rf, uint8_t reg);
uint8_t rf24_write_register(rf24_t *rf, uint8_t reg, uint8_t* buf, uint8_t len);
uint8_t rf24_write_register2(rf24_t *rf, uint8_t reg, uint8_t value);
uint8_t rf24_getPALevel(rf24_t *rf);
void rf24_setPALevel(rf24_t *rf, uint8_t level);
bool rf24_setDataRate(rf24_t *rf, uint8_t speed);
uint8_t rf24_getDataRate(rf24_t *rf);
void rf24_setCRCLength(rf24_t *rf, rf24_crclength_e length);
rf24_crclength_e rf24_getCRCLength(rf24_t *rf);
void rf24_setChannel(rf24_t *rf, uint8_t channel);
uint8_t rf24_get_status(rf24_t *rf);
uint8_t rf24_create(rf24_t *rf, pp_t *_cepin, pp_t *_cspin);
uint8_t rf24_flush_rx(rf24_t *rf);
uint8_t rf24_flush_tx(rf24_t *rf);
uint8_t rf24_begin(rf24_t *rf);
void rf24_PayloadSize(rf24_t *rf, uint8_t size);
uint8_t rf24_getPayloadSize(rf24_t *rf);
void rf24_startListening(rf24_t *rf);
void rf24_powerDown(rf24_t *rf);
void rf24_powerUp(rf24_t *rf);
uint8_t rf24_write_payload(rf24_t *rf, const void* buf, uint8_t len);
void rf24_startWrite(rf24_t *rf, const void* buf, uint8_t len );
void rf24_whatHappened(rf24_t *rf, bool *tx_ok, bool *tx_fail, bool *rx_ready);
void rf24_setRetries(rf24_t *rf, uint8_t delay, uint8_t count);
void rf24_setPayloadSize(rf24_t *rf, uint8_t size);
void rf24_openWritingPipe(rf24_t *rf, uint8_t *value);
void rf24_openReadingPipe(rf24_t *rf, uint8_t child, uint8_t *address);
uint8_t rf24_getDynamicPayloadSize(rf24_t *rf);
bool rf24_write(rf24_t *rf, const void* buf, uint8_t len );
uint8_t rf24_read_payload(rf24_t *rf, void* buf, uint8_t len);
bool rf24_available(rf24_t *rf, uint8_t* pipe_num);
bool rf24_read(rf24_t *rf, void* buf, uint8_t len );




#endif // rf24_h
