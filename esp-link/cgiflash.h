#ifndef CGIFLASH_H
#define CGIFLASH_H

#include "httpd.h"

int ICACHE_FLASH_ATTR cgiReadFlash(HttpdConnData *connData);
int ICACHE_FLASH_ATTR cgiGetFirmwareNext(HttpdConnData *connData);
int ICACHE_FLASH_ATTR cgiUploadFirmware(HttpdConnData *connData);
int ICACHE_FLASH_ATTR cgiRebootFirmware(HttpdConnData *connData);
int ICACHE_FLASH_ATTR cgiReset(HttpdConnData *connData);

#endif
