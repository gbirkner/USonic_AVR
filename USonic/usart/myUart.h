/* 
* uart.h
*
* Created: 06.07.2017 14:02:00
* Author: gert
*/


#ifndef __MYUART_H__
#define __MYUART_H__

#include <avr/io.h>
#include <stdlib.h>
#include "../General.h"


#define UART_LF	'\n'
#define UART_CR '\r'


class myUart {

//variables
public:
	
private:
	const uint16_t UBRR_VAL = 51; // ((F_CPU+BAUD*8)/(BAUD*16)-1) => 19600

//functions
public:
	myUart();
	~myUart();

	void enable();
	void disable();
	void write(unsigned char c, bool linefeed = false);
	void write(char *s, bool linefeed = false);
	void write(uint16_t i, bool linefeed = false);
	void write(int16_t i, bool linefeed = false);
	void write(uint32_t i, bool linefeed = false);
	void write(int32_t i, bool linefeed = false);
	void write(float f, uint8_t precision = 2, bool linefeed = false);
	void write(double d, uint8_t precision = 2, bool linefeed = false);
	void cls();
	void LfCr();
	uint8_t read(unsigned char *x, uint8_t size);
	unsigned char read();


private:
	myUart( const myUart &c );
	myUart& operator=( const myUart &c );

};   //myUart

extern myUart uart;



#endif //__MYUART_H__
