#This file was modified by Rtzz0

OBJECT=blank.ihx

SDCC=sdcc
SDLD=sdld

PROG=stlinkv2
PART=stm8s103p3
CPUFREQ=16000000


# dependencies with relative folders
LIBS+=../inc/common.c
LIBS+=../lib/uart_proto/proto.c
#LIBS+=../lib/serializer/serializer.c
#LIBS+=../lib/twi/twi.c
#LIBS+=../lib/stm8s-sdcc/src/stm8s_tim4.c
#LIBS+=../lib/stm8s-sdcc/src/stm8s_clk.c
#LIBS+=../lib/stm8s-sdcc/src/stm8s_spi.c
#LIBS+=../lib/pcf/pcf8574.c
#LIBS+=../lib/pcf/7seg.c
LIBS+=../lib/millis/tim4millis.c
LIBS+=../lib/uart.c


# Main source files (files in this folder)
CSOURCES=blank.c $(LIBS)

# Change serializer address for parts other than stm8s103
INCLUDEDIRS= -D F_CPU=$(CPUFREQ) -D SERIALIZER_ADDRESS=0x9E00 -D PROTO_SERVER=1

include ../inc/includedirs.mk

include ../inc/common.mk
