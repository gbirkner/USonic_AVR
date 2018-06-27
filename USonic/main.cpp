/*
 * USonic.cpp
 *
 * Created: 07.06.2018 21:35:20
 * Author : gert
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "general.h"
#include "util/delay.h"
#include "usart/myUart.h"
#include "usonic/usonic.h"

myUart uart;

int main(void)
{
	sei();
	//DEBUG
	DDRA |= (1 << PA1) | (1 << PA2);


	uint16_t x = 0; 
	uart.enable();
	uart.cls();
	uart.write((char*)"Ultrasonic distance meassurement -----------------\0", true);
	uart.LfCr();
	
    
    while (1) 
    {
		x = usonic::getDistance(200);
		uart.write((char*)"d=\0"); uart.write(x); uart.write((char*)"mm\0", true);
		_delay_ms(1000);
    }
}

