/* 
* usonic.h
*
* Created: 07.06.2018 21:38:57
* Author: gert birkner
* Description:
*   Interrupt driven HC-SR04 ultrasonic driver
*   Uses 1 timer to set and delete the trigger signal
*   and the same timer to meassure the echo signal
*	For better Overview in bigger projects copy the ISRs from the usonic.cpp in another file 
*   with all the other ISRs of your project so you can check if there is a conflict.
*   If you use board.h files which contains all defines for Ports, pins, ... 
*   you can copy the defines from usonic.h into this file 
*
*	Example: 
*   main.cpp:   
*   #include "usonic/usonic.h"
*   
*   in main()
*   uint16_t x = usonic::getDistance(200);	// to get the average distance from 10 meassurements at a given temperatur (here 20°C)
*   
*   if (usonic::isReady())					// to check if the meassurement is completed
*
*	don´t use: 
*	static uint8_t getOverflow();
*	static void setOverflow(uint8_t Overflow);
*	static void calcDistance(uint16_t timerValue);
*	they are public because they are used in global ISRs but they are for internal use only!
*/


#ifndef __USONIC_H__
#define __USONIC_H__

#include <avr/io.h>


#define CSchall	331.6f		// sonicvelocity at 0°C


class usonic
{
//variables
private:
	static uint8_t _overflow;		// counter for timeroverflows to count bigger time differences
	static uint16_t _temp;			// temperatur in °C * 10 -> 22°C = 220
	static bool _isReady;			// false during meassuring, true when correct value is available
	static uint8_t _MCount;			// meassure count
	static uint16_t _arrResult[10];	// array of distance results (mm)
	static bool _isMeassuring;		// false from setTrigger() to calcDistance() 

//functions
public:
	static uint16_t getDistance(uint16_t temp);
	static bool isReady();
	static uint8_t getOverflow();
	static void setOverflow(uint8_t Overflow);
	static void calcDistance(uint16_t timerValue);

private:
	static void sortArray();
	static uint16_t getAverageDistance();
	static void setTrigger();
}; //usonic






#endif //__USONIC_H__
