#ifndef EEPROM_H
#define EEPROM_H

#include <stm8s.h>

void eeprom_read(uint8_t *address, uint8_t *data, uint16_t len);
void eeprom_write(uint8_t *address, uint8_t *data, uint16_t len);

#endif
