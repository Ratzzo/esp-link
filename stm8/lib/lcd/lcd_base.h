#ifndef LiquidCrystal_h
#define LiquidCrystal_h

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <common.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

typedef void (*lcd_bitset_funct_t)(void *lcd, uint8_t value, uint8_t mode);

typedef struct lcd_t {
	lcd_bitset_funct_t bitset_func;
	uint8_t _rs_pin; // LOW: command.  HIGH: character.
	uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
	uint8_t _enable_pin; // activated by a HIGH pulse.
	uint8_t _data_pins[8];


	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;

	uint8_t _initialized;

	uint8_t _numlines,_currline;
} lcd_t;

void lcd_create(lcd_t *lcd, lcd_bitset_funct_t bitset_func, uint8_t rs,  uint8_t enable,
				   uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

void lcd_init(lcd_t *lcd, uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
			 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
size_t lcd_printf(lcd_t *lcd, const char *format, ...);
void lcd_setCursor(lcd_t *lcd, uint8_t col, uint8_t row);
void lcd_command(lcd_t *lcd, uint8_t value);

void lcd_noDisplay(lcd_t *lcd);
void lcd_pinMode(lcd_t *lcd, uint8_t value, uint8_t mode);
void bitwrite_func(lcd_t *lcd, uint8_t value, uint8_t mode);
void lcd_begin(lcd_t *lcd, uint8_t lines, uint8_t dotsize);
void lcd_write8bits(lcd_t *lcd, uint8_t value);
void lcd_write4bits(lcd_t *lcd, uint8_t value);
//void lcd_command(lcd_t *lcd, uint8_t value);
size_t lcd_write(lcd_t *lcd, uint8_t value);
void lcd_display(lcd_t *lcd);
void lcd_clear(lcd_t *lcd);
void lcd_send(lcd_t *lcd, uint8_t value, uint8_t mode);
void lcd_createChar(lcd_t *lcd, uint8_t location, uint8_t *charmap);

#endif
