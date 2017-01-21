#ifndef LCD_H
#define	LCD_H

#pragma warning disable 520
#pragma warning disable 752

#include <stdlib.h>
#include <xc.h>

//Oscillator frequency must be defined for delays
//Define in Properties -> XC8 compiler -> Define macros: _XTAL_FREQ=16000000UL

//4 bit bus for the data lines
//Should be set to output in pin configuration
#define D4 LATAbits.LA0
#define D5 LATAbits.LA1
#define D6 LATAbits.LA2
#define D7 LATAbits.LA3

//Latches for control pins - enable, read/write, register select
//Should be set to output in pin configuration
#define RS LATAbits.LA5
#define EN LATAbits.LA4

//Digit buffer for integer send functions
#define DIGIT_BUFFER 10

void lcd_sendCommand(char a);

void lcd_clear();

void lcd_setPosition(unsigned char x, unsigned char y);

void lcd_init(void);

void lcd_sendChar(const char a);

void lcd_sendString(const char * a);

void lcd_sendStringToPos(unsigned char x, unsigned char y, const char * charString);

void lcd_sendInt(int number);

void lcd_sendIntToPos(unsigned char x, unsigned char y, int number);

void lcd_sendFloat(float number);

void lcd_sendFloatToPos(unsigned char x, unsigned char y, float number);

void lcd_sendUInt(unsigned int number);

void lcd_sendUIntToPos(unsigned char x, unsigned char y, unsigned int number);

void lcd_shiftRight();

void lcd_shiftLeft();

void _lcd_Port(char a);


#endif	/* LCD_H */

