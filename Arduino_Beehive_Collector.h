/*
 * Arduino_Beehive_Collector.h
 *
 *  Created on: Oct 4, 2014
 *      Author: nuptse
 */

#ifndef ARDUINO_BEEHIVE_COLLECTOR_H_
#define ARDUINO_BEEHIVE_COLLECTOR_H_

#include <Arduino.h>
#include "Arduino_Beehive_Scale_MBMap.h"

//////////////////// Port information ///////////////////
#define BAUD 9600
#define TIMEOUT 1000
#define POLLING 1000 // the scan rate

// If the packets internal retry register matches
// the set retry count then communication is stopped
// on that packet. To re-enable the packet you must
// set the "connection" variable to true.
#define RETRY_COUNT 10

// used to toggle the receive/transmit pin on the driver
#define RX_ENABLE_PIN 12
#define TX_ENABLE_PIN  9
/////////////////////////////////////////////////////////

unsigned int holdingRegs[HOLDING_REGS_SIZE];

#define DEBUG

#ifdef DEBUG

void print_debug( unsigned long ulTime, const char * szMessaqe )
{
	if( 0x0000 != holdingRegs[DEBUG_ENABLE] )
	{
		Serial.print(ulTime) ;
		Serial.println(szMessaqe);
	}
}

#undef PDEBUG // Just in case
#ifndef PDEBUG
	#define PDEBUG(time, msg) \
		print_debug( time, msg)
#else
	#define PDEBUG(time, msg) /* nothing */
#endif

#endif // DEBUG

#endif /* ARDUINO_BEEHIVE_COLLECTOR_H_ */
