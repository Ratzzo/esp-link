#include <eeprom.h>

void eeprom_write(uint8_t *address, uint8_t *data, uint16_t len)
{
    //  Check if the EEPROM is write-protected.  If it is then unlock the EEPROM.

    if (!(FLASH->IAPSR & FLASH_IAPSR_DUL)) {
        FLASH->DUKR = 0xAE;
        FLASH->DUKR = 0x56;
    }

    //  Write the data to the EEPROM.

    address += 0x4000;
    while(len--)
        *address++ = *data++;

    //  Now write protect the EEPROM.
    FLASH->IAPSR &= ~FLASH_IAPSR_DUL;
}

void eeprom_read(uint8_t *address, uint8_t *data, uint16_t len)
{
    //  Write the data to the EEPROM.
    address += 0x4000;
    while(len--)
        *data++ = *address++;
}
