// Copyright (c) 2016-2017 by Danny Backx, see LICENSE.txt in the esp-link repo

#ifndef CGIMEGA_H
#define CGIMEGA_H

#include <httpd.h>


int  cgiMegaSync(HttpdConnData *connData);
int  cgiMegaData(HttpdConnData *connData);
int  cgiMegaRead(HttpdConnData *connData);
int  cgiMegaFuse(HttpdConnData *connData);
int  cgiMegaRebootMCU(HttpdConnData *connData);

#endif
