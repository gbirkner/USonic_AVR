/* 
* usonic.cpp
*
* Created: 07.06.2018 21:38:57
* Author: gert
*/


#include "usonic.h"
#include "../general.h"
#include <avr/interrupt.h>
#include <stdint.h>

uint8_t usonic::_overflow;
uint16_t usonic::_temp;
bool usonic::_isReady;
uint8_t usonic::_MCount;
bool usonic::_isMeassuring;
uint16_t usonic::_arrResult[10];

void usonic::setTrigger() {
	_isMeassuring = true;
	TCNT1 = 0;
	OCR1A = 63999;				// 4ms at 16MHz for 40ms trigger impuls (mind. 10µs) to avoid double echos (>6m) -> 10 x overflow
	_overflow = 0;				// reset overflow
	TIMSK1 |= ( 1 << OCIE1A);	// activate COMP A interrupt
	TCCR1B |= (1 << CS10);		// Prescaler 1 -> start timer
	PORTA |= (1 << PA0);		// set Trigger pin
}

uint8_t usonic::getOverflow() {
	return _overflow;
}

void usonic::setOverflow(uint8_t Overflow) {
	_overflow = Overflow;
}

void usonic::calcDistance(uint16_t timerValue) {
	uint64_t result = (uint64_t)timerValue + ((uint64_t)_overflow * 65535);		// time in nanoseconds * 10: (1/F_CPU) * 10^10 (62.5ns)
	result = (((result * 625) * (33160 + (6 * (uint64_t)_temp)))/2/1000000000);	// distance in mm	-> CShall = 331.6 + (0.6 * temp)				
	
	if (_MCount < 10) {
		_arrResult[_MCount] = (uint16_t)result;
		_MCount++;
	}
	_isMeassuring = false;
}


uint16_t usonic::getDistance(uint16_t temp) {
	// initialization
	_overflow = 0;
	_temp = 200;
	_isReady = true;
	_MCount = 0;
	_isMeassuring = false;
	PORTA &= ~(1 << PA0);
	DDRA |= (1 << PA0);		// Triggerport/Pin to output;
	DDRD |= ~(1 << PD3);
										
	TCCR1A &= ~((1 << WGM11) | ( 1 << WGM10));				// Timermode to normal
	TCCR1B &= ~((1 << WGM13) | ( 1 << WGM12));
	TCCR1B &= ~((1 << CS10) | ( 1 << CS11) | ( 1 << CS12)); // Timer stop
	TIMSK1 |= (1 << TOIE1);									// activate Timer overflow interrupt
	EICRA &= ~(1 << ISC31);									// extern Interrupt setup for raising and falling edge
	EICRA |= (1 << ISC30);


	_isReady = false;
	while (_MCount < 10) {
		setTrigger();
		while(_isMeassuring) {
		}
	}
	sortArray();
	uint16_t result = getAverageDistance();
	_isReady = true;
	return result;
}


bool usonic::isReady() {
	return _isReady;
}

void usonic::sortArray() {
	bool bSorted = false;
	while (bSorted == false)
	{
		bSorted = true;
		for (int i = 0; i < 10; ++i)
		{
			uint16_t distance1 = _arrResult[i];
			uint16_t distance2 = _arrResult[i + 1];
			if (distance1 > distance2)
			{
				_arrResult[i] = distance2;
				_arrResult[i + 1] = distance1;
				bSorted = false;
			}
		}
	}
}


uint16_t usonic::getAverageDistance() {

	uint16_t compareValue = (_arrResult[4] + _arrResult[5]) / 2;
	uint8_t lowIndex = 0, highIndex = 9;

	for (uint8_t i = 0; i < 10; ++i) {
		if (_arrResult[i] < (uint32_t)compareValue * 95 / 100)	{
			lowIndex = i;
		} else if (_arrResult[i] > (uint32_t)compareValue * 105 / 100)	{
			highIndex = i + 1;
			break;
		}
	}
	uint32_t averageValue = 0;
	for (uint8_t i = lowIndex; i < highIndex; ++i)	{
		averageValue += _arrResult[i];
	}
	return averageValue / (highIndex - lowIndex);
}

ISR(INT3_vect) {	
	PORTA ^= (1 << PA2);
	if(PIND & (1 << PD3)) {										// raising edge (starting meassure)
		TCCR1B |= (1 << CS10);									// Prescaler 1 -> start timer
	} else {													// falling edge
		TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));	// stop Timer
		EIMSK &= ~(1 << INT3);									// deactivate extern interrupt sense
		usonic::calcDistance(TCNT1);							// calculate the distance
	}
}

ISR(TIMER1_COMPA_vect) {
	//DEBUG
	PORTA ^= (1 << PA1);

	uint8_t ovf = usonic::getOverflow();
	ovf++;
	usonic::setOverflow(ovf);				// increment overflow for 40ms trigger impuls
	if (ovf >= 1) {							// 10 overflow (40ms) reached?
		TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));	// stop Timer	
		PORTA &= ~(1 << PA0);									// delete Trigger pin
		TIMSK1 &= ~(1 << OCIE1A);								// deactivate Trigger COMPA Interrupt
		usonic::setOverflow(0);
		EIMSK |= (1 << INT3);									// activate extern interrupt sense	
	}	
	TCNT1 = 0;	
}

ISR(TIMER0_OVF_vect) {	

	usonic::setOverflow(usonic::getOverflow()+1);
}