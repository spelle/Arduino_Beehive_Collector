/*
 * Arduino_Beehive_Scale.cpp
 *
 *  Created on: Oct 3, 2014
 *      Author: spelle
 */



#include <Arduino.h>
#include <SimpleSWModbusMaster.h>
#include "SoftwareSerial.h"
//#include <EEPROM.h>

#include <avr/wdt.h>

#include "Arduino_Beehive_Collector.h"

#define DISPLAY_MODBUS_PERIOD 1000
#define BLINK_LED_PERIOD 1000

SoftwareSerial SWSerial(10, 11) ; // RX, TX

// If the packets internal retry register matches
// the set retry count then communication is stopped
// on that packet. To re-enable the packet you must
// set the "connection" variable to true.
#define retry_count 10

#define RX_ENABLE_PIN 12
#define TX_ENABLE_PIN  9

// This is the easiest way to create new packets
// Add as many as you want. TOTAL_NO_OF_PACKETS
// is automatically updated.
enum
{
  PACKET_ID,
  PACKET_GAUGES,
  TOTAL_NO_OF_PACKETS // leave this last entry
};

// Create an array of Packets to be configured
Packet packets[TOTAL_NO_OF_PACKETS];

// Create a packetPointer to access each packet
// individually. This is not required you can access
// the array explicitly. E.g. packets[PACKET1].id = 2;
// This does become tedious though...
packetPointer packet_ID = &packets[PACKET_ID];
packetPointer packet_GAUGES = &packets[PACKET_GAUGES];

// Data read from the arduino slave will be stored in this array
// if the array is initialized to the packet.
#define READ_REG_ID_SIZE 1
unsigned int readReg_ID[READ_REG_ID_SIZE];
#define READ_REGS_GAUGES_SIZE 3
unsigned int readRegs_GAUGES[READ_REGS_GAUGES_SIZE];

// Data to be written to the arduino slave
unsigned int writeReg[1];


void setup()
{
	// Set PB5 as OUTPUT
	DDRB |= (1 << PB5 ) ;

	// The modbus packet constructor function will initialize
	// the individual packet with the assigned parameters. You can always do this
	// explicitly by using struct pointers. The first parameter is the address of the
	// packet in question. It is effectively the "this" parameter in Java that points to
	// the address of the passed object. It has the following form:
	// modbus_construct(packet, id, function, address, data, register array)

	// For functions 1 & 2 data is the number of points
	// For functions 3, 4 & 16 data is the number of registers
	// For function 15 data is the number of coils

	// read 1 register starting at address 0
	modbus_construct(packet_ID, 2, READ_HOLDING_REGISTERS, ID, 1, readReg_ID);
	modbus_construct(packet_GAUGES, 2, READ_HOLDING_REGISTERS, GAUGE1_RAW, 3, readRegs_GAUGES);

	// write 1 register starting at address 1
	//modbus_construct(readRegs_GAUGES, 2, PRESET_MULTIPLE_REGISTERS, 1, 1, writeRegs);

	// P.S. the register array entries above can be different arrays

	/* Initialize communication settings:
		parameters(HardwareSerial* SerialPort,
		long baud,
		unsigned char byteFormat,
		unsigned int timeout,
		unsigned int polling,
		unsigned char retry_count,
		unsigned char TxEnablePin,
		Packet* packets,
		unsigned int total_no_of_packets);

		Valid modbus byte formats are:
		SERIAL_8N2: 1 start bit, 8 data bits, 2 stop bits
		SERIAL_8E1: 1 start bit, 8 data bits, 1 Even parity bit, 1 stop bit
		SERIAL_8O1: 1 start bit, 8 data bits, 1 Odd parity bit, 1 stop bit

		You can obviously use SERIAL_8N1 but this does not adhere to the
		Modbus specifications. That said, I have tested the SERIAL_8N1 option
		on various commercial masters and slaves that were suppose to adhere
		to this specification and was always able to communicate... Go figure.

		These are already defined in the Arduino global name space.
	 */
	modbus_configure(&SWSerial, BAUD, /*SERIAL_8N2,*/ TIMEOUT, POLLING, RETRY_COUNT, TX_ENABLE_PIN, packets, TOTAL_NO_OF_PACKETS);


	// Initialize HoldingRegs area
	memset( readReg_ID, 0, READ_REG_ID_SIZE * sizeof(unsigned int)) ;
	memset( readRegs_GAUGES, 0, READ_REGS_GAUGES_SIZE * sizeof(unsigned int)) ;

	Serial.begin(115200);

	Serial.println("--- Arduino Beehive Collector ---");
}

void loop()
{
	modbus_update() ;

	static unsigned long ulCurrentTime = 0 ;
	static unsigned long ulNextTimeDisplay = DISPLAY_MODBUS_PERIOD ;
	static unsigned long ulNextTimeLED = BLINK_LED_PERIOD ;

	ulCurrentTime = millis() ;

	if( ulCurrentTime > ulNextTimeDisplay )
	{
		Serial.print( ulCurrentTime ) ;
		Serial.print( ": Beehive Scale ID : " ) ;
		Serial.println( readReg_ID[0] ) ;

		Serial.print( ulCurrentTime ) ;
		Serial.print( ": Beehive Gauge #1 : " ) ;
		Serial.println( readRegs_GAUGES[0] ) ;

		Serial.print( ulCurrentTime ) ;
		Serial.print( ": Beehive Gauge #2 : " ) ;
		Serial.println( readRegs_GAUGES[1] ) ;

		Serial.print( ulCurrentTime ) ;
		Serial.print( ": Beehive Gauge #3 : " ) ;
		Serial.println( readRegs_GAUGES[2] ) ;

		ulNextTimeDisplay = ulCurrentTime + DISPLAY_MODBUS_PERIOD ;
	}

	if( ulCurrentTime > ulNextTimeLED )
	{
		PORTB ^= (1 << PB5 ) ;

		ulNextTimeLED = ulCurrentTime + BLINK_LED_PERIOD ;
	}
}




