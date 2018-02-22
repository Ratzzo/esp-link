#ifndef SOFT_RESET_H
#define SOFT_RESET_H

#ifndef SP_ON_RESET
#error Upper stack on reset not defined, define it for your device #define SP_ON_RESET
#else
#define _HASH #
#define HASH() _HASH
#define soft_reset()	__asm \
							ldw x, HASH()SP_ON_RESET	/*set x to upper stack limit */		\
							ldw sp, x					/*set sp as x */					\
							ldw x, HASH()0x8000			/* load the reset vector into x */	\
							jp (x)						/* call the reset vector */			\
						__endasm
#endif

#endif
