#include "serializer.h"

uint8_t serializer_byte;
uint8_t serializer_count;


#define serializer_resnop     nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop


#define serializer_setnop     nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop \
nop


//this places the stub at the address on .org
void serializedata_section() __naked {
    __asm
    .area ABSCODE (ABS, CON)
    .org SERIALIZER_ADDRESS
    __endasm;
}

void serializedata(uint8_t *data, uint8_t size) __naked {
    data;
    size;
__asm;
    clrw x
    jrt _continue
    _exit:
    ret
    _continue:
    ldw x, (0x03, sp) //data
    ldw y, #_serializer_count
    ld a, (0x05, sp) //size
    ld (y), a
    nop //padding
    nop
    nop
    inc (y)
    back$: //this is not very stable timing wise, alignment is key.
    dec (y)
    jreq _exit
    ld a, (x)
    incw x
    ld _serializer_byte, a

    btjt _serializer_byte, #0, set0
    bres 0x500F, #2
    serializer_resnop
    jrt noset0
    set0:
    bset 0x500F, #2
    serializer_setnop
    noset0:

    btjt _serializer_byte, #1, set1
    bres 0x500F, #2
    serializer_resnop
    jrt noset1
    set1:
    bset 0x500F, #2
    serializer_setnop
    noset1:

    btjt _serializer_byte, #2, set2
    bres 0x500F, #2
    serializer_resnop
    jrt noset2
    set2:
    bset 0x500F, #2
    serializer_setnop
    noset2:

    btjt _serializer_byte, #3, set3
    bres 0x500F, #2
    serializer_resnop
    jrt noset3
    set3:
    bset 0x500F, #2
    serializer_setnop
    noset3:

    btjt _serializer_byte, #4, set4
    bres 0x500F, #2
    serializer_resnop
    jrt noset4
    set4:
    bset 0x500F, #2
    serializer_setnop
    noset4:

    btjt _serializer_byte, #5, set5
    bres 0x500F, #2
    serializer_resnop
    jrt noset5
    set5:
    bset 0x500F, #2
    serializer_setnop

    noset5:

    btjt _serializer_byte, #6, set6
    bres 0x500F, #2
    serializer_resnop
    jrt noset6
    set6:
    bset 0x500F, #2
    serializer_setnop
    noset6:

    btjt _serializer_byte, #7, set7
    bres 0x500F, #2
    nop
    nop
    nop
    nop
    nop
    jrt noset7
    set7:
    bset 0x500F, #2
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    noset7:

    jp back$
    .area CODE (REL, CON)
__endasm;
}
