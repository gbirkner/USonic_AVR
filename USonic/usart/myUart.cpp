/* 
* uart.cpp
*
* Created: 06.07.2017 14:01:59
* Author: gert
*/


#include "myUart.h"


/**
 * \brief 
 *  write single u char when UART0 is ready to use
 * \param c
 *  char or uint8_t (ASCII) to send
 * \param linefeed
 *  if true ... send CR and LF
 *  default value = false
 * \return void
 */
void myUart::write(unsigned char c, bool linefeed /*= false*/) {
	while(!(UCSR0A & (1 << UDRE0))) {	}
	UDR0 = c;
	if (linefeed) {
		LfCr();
	}

}

/**
 * \brief 
 *  send string as char array (call every single char from array
 *  char array must be '\0' terminated
 * \param s
 *  char array '\0' terminated
 * \param linefeed
 *  if true .. send CR and LF
 *  default value = false
 * \return void
 */
void myUart::write(char *s, bool linefeed /*= false*/) {
	while (*s) {
		write((unsigned char)*s, false);
		s++;
	}
	if (linefeed) LfCr();
}


/**
 * \brief 
 *  convert int16_t to char array and call write for char arrays, incl. algebraic sign
 * \param i
 *  integer to send 
 * \param linefeed
 *  is handed over to write for char arrays; 
 *  default value = false
 * \return void
 */
void myUart::write(int16_t i, bool linefeed /*= false*/) {
	char s[7];
	itoa(i, s, 10);
	write(s, linefeed);
}

/**
 * \brief 
 *  convert uint16_t to char array and call write for char arrays
 * \param i
 *  integer to send
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void myUart::write(uint16_t i, bool linefeed /*= false*/) {
	char s[6];
	itoa(i, s, 10);
	write(s, linefeed);	
}

/**
 * \brief 
 *  convert uint32_t to char array and call write for char arrays
 * \param i
 *  integer to send
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void myUart::write(uint32_t i, bool linefeed /*= false*/) {
	char s[8];
	itoa(i, s, 10);
	write(s, linefeed);
}

/**
 * \brief 
 *  convert int32_t to char array and call write for char arrays incl algebraic sign
 * \param i
 *  integer to send
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void myUart::write(int32_t i, bool linefeed /*= false*/) {
	char s[9];
	write(itoa(i, s, 10), linefeed);	
}


/**
 * \brief 
 *  convert float to char array and call write for char arrays incl. algebraic sign
 * \param f
 *  float value to send
 * \param precision
 *  number of decimal places to enclose in array -> and send
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void myUart::write(float f, uint8_t precision, bool linefeed /*= false*/) {
	char s[10];
	write(dtostrf(f, 7, precision, s), linefeed);
}


/**
 * \brief 
 *  convert double to char array and call write for char arrays incl. algebraic sign
 * \param d
 *  float value to send
 * \param precision
 *  number of decimal places to enclose in array -> and send
 *  default value is 2 digits
 * \param linefeed
 *  is handed over to write for char arrays
 *  default value is false
 * \return void
 */
void myUart::write(double d, uint8_t precision /*= 2*/, bool linefeed /*= false*/) {
	char s[12];
	write(dtostrf(d, 9, precision, s), linefeed);
}


/**
 * \brief 
 *  default constructor 
 *  inits the UART0 Interface -> Baud, 8 Databits, 1 stop, no parity
 * 
 * \return 
 */
myUart::myUart() {
  UBRR0H = (uint8_t)(UBRR_VAL >> 8);
  UBRR0L = (uint8_t)UBRR_VAL;
  UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00); // Asynchron 8N1
} //uart

// default destructor
myUart::~myUart() {
} //~uart



/**
 * \brief 
 *  reads chars from UART0 into an array until \n or \r is hit or size -1 is reached
 *  and sets an final \0
 * \param x
 *  char array
 * \param size
 *  max. number of chars
 *
 * \return uint8_t
 *  number of read chars without \0
 */
uint8_t myUart::read(unsigned char *x, uint8_t size) {
	uint8_t i = 0;
	if (size == 0) return 0;				// return 0 if no space
	unsigned char c = 0;
	while (i < (size - 1)) {				// check space is available (including additional null char at end)
		while ( !(UCSR0A & (1<<RXC0)) );	// wait for another char - WARNING this will wait forever if nothing is received
		c = UDR0;		
		if (c == '\n' || c == '\r') break;  // break on NULL OR ENTER character
		x[i] = c;							// write into the supplied buffer
		i++;
	}
	if (i >= size) i = size - 1;// ensure the last character in the array is '\0'
	x[i] = '\0';	
	return i;				// return number of characters written (incl. '\0')
}


/**
 * \brief 
 *  reads a single char from UART
 * 
 * \return unsigned char
 */
unsigned char myUart::read() {
	while( !(UCSR0A & (1<<RXC0)));
	return UDR0;
}


/**
 * \brief 
 *  enables UART0 recieve and transmit modes
 * 
 * \return void
 */
void myUart::enable() {
	  UCSR0B |= (1<<RXEN0) | (1<<TXEN0);  // UART RX, TX 
}


/**
 * \brief 
 *  disables recieve and transmitt for UART0
 * 
 * \return void
 */
void myUart::disable() {
	UCSR0B &= ~((1<<RXEN0) | (1<<TXEN0));  // UART RX, TX 
}


/**
 * \brief 
 *  send clear screen command for VT100 Terminal
 *  with Escape sequences
 * 
 * \return void
 */
void myUart::cls() {
	write((unsigned char)(27));
	write((unsigned char)('['));
	write((unsigned char)('2'));
	write((unsigned char)('J'));
	write((unsigned char)(27));
	write((unsigned char)('['));
	write((unsigned char)('H'));
}



/**
 * \brief 
 *  send CR and LF sequence for VT100 Terminal
 *  to display proper Linefeed
 * 
 * \return void
 */
void myUart::LfCr() {
	while(!(UCSR0A & (1 << UDRE0))) {	}
	UDR0 = ((unsigned char)UART_LF);
	while(!(UCSR0A & (1 << UDRE0))) {	}
	UDR0 = ((unsigned char)UART_CR);
}