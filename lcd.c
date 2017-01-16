#include "lcd.h"

void lcd_sendCommand(char a)
{
	RS = 0;             // => RS = 0
	_lcd_Port(a);
	EN  = 1;             // => E = 1
        __delay_ms(4);
        EN  = 0;             // => E = 0
}

void lcd_clear()
{
	lcd_sendCommand(0);
	lcd_sendCommand(1);
}

void lcd_setPosition(unsigned char x, unsigned char y)
{
	unsigned char temp, i, j;
	if(y == 1)
	{
        temp = 0x80 + x - 1;
		i = temp>>4;
		j = temp & 0x0F;
		lcd_sendCommand(i);
		lcd_sendCommand(j);
	}
	else if(y == 2)
	{
		temp = 0xC0 + x - 1;
		i = temp>>4;
		j = temp & 0x0F;
		lcd_sendCommand(i);
		lcd_sendCommand(j);
	}
}

void lcd_init()
{
  _lcd_Port(0x00);
   __delay_ms(20);
  lcd_sendCommand(0x03);
	__delay_ms(5);
  lcd_sendCommand(0x03);
	__delay_ms(11);
  lcd_sendCommand(0x03);
  /////////////////////////////////////////////////////
  lcd_sendCommand(0x02);
  lcd_sendCommand(0x02);
  lcd_sendCommand(0x08);
  lcd_sendCommand(0x00);
  lcd_sendCommand(0x0C);
  lcd_sendCommand(0x00);
  lcd_sendCommand(0x06);
}

void lcd_sendChar(const char a)
{
   char temp,y;
   temp = a&0x0F;
   y = a&0xF0;
   RS = 1;             // => RS = 1
   _lcd_Port(y>>4);    //Data transfer
   EN = 1;
   __delay_us(40);
   EN = 0;
   _lcd_Port(temp);
   EN = 1;
   __delay_us(40);
   EN = 0;
}

void lcd_sendString(const char * a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	   lcd_sendChar(a[i]);
}

void lcd_sendStringToPos(unsigned char x, unsigned char y, const char * charString)
{
	lcd_setPosition(x, y);
	lcd_sendString(charString);
}

void lcd_sendInt(int number)
{
	char displayString[DIGIT_BUFFER];
    itoa(displayString, number, 10);
	lcd_sendString(displayString);
}

void lcd_sendIntToPos(unsigned char x,unsigned char y, int number)
{
	lcd_setPosition(x, y);
	lcd_sendInt(number);
}

void lcd_sendFloat(float number)
{
    char * buffer;
    int status;
    buffer = ftoa(number, &status);
    lcd_sendString(buffer);
}

void lcd_sendFloatToPos(unsigned char x, unsigned char y, float number)
{
    lcd_setPosition(x, y);
	lcd_sendFloat(number);
}

void lcd_sendUInt(unsigned int number)
{
	char displayString[DIGIT_BUFFER];
    ultoa(displayString, number, 10);
	lcd_sendString(displayString);
}

void lcd_sendUIntToPos(unsigned char x, unsigned char y, unsigned int number)
{
    lcd_setPosition(x, y);
	lcd_sendUInt(number);
}

void lcd_shiftRight()
{
	lcd_sendCommand(0x01);
	lcd_sendCommand(0x0C);
}

void lcd_shiftLeft()
{
	lcd_sendCommand(0x01);
	lcd_sendCommand(0x08);
}

void _lcd_Port(char a)
{
	if(a & 1)
		D4 = 1;
	else
		D4 = 0;

	if(a & 2)
		D5 = 1;
	else
		D5 = 0;

	if(a & 4)
		D6 = 1;
	else
		D6 = 0;

	if(a & 8)
		D7 = 1;
	else
		D7 = 0;
}
