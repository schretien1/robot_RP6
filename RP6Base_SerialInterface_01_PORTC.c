/* 
 * ****************************************************************************
 * RP6 ROBOT SYSTEM - ROBOT BASE EXAMPLES
 * ****************************************************************************
 * Example: Serial Interface Example - writing Numbers
 * Author(s): Dominik S. Herwald
 * ****************************************************************************
 * Description:
 * This sample program shows how to use some of the integer output serial 
 * interface functions of the RP6Library (s. also "RP6uart.c"). 
 * The writeInteger function is very important because you can
 * output sensor values over the serial interface with it.
 * This is very useful to solve problems in your programs or optimize some
 * parameters.
 *
 * Here we only output a counter value every 100ms.
 *
 * ############################################################################
 * The Robot does NOT move in this example! You can simply put it on a table
 * next to your PC and you should connect it to the PC via the USB Interface!
 * ############################################################################
 * ****************************************************************************
 */

/*****************************************************************************/
// Includes:

#include "RP6RobotBaseLib.h" 	// The RP6 Robot Base Library.
//#include "RP6ControlLib.h"		// Always needs to be included!

/*****************************************************************************/
// Main function - The program starts here:

int main(void)
{
	initRobotBase();// Always call this first! The Processor will not work
					 // correctly otherwise.
	//initRP6Control(); 				 
	setLEDs(0b111111); // Turn all LEDs on
	mSleep(500);       // delay 500ms
	setLEDs(0b000000); // All LEDs off

	// Write a text message to the UART:
	writeString_P("\nJust a simple counter program\n\n");
	
	// Define a counting variable:
	uint16_t counter = 0;
	//uint16_t adc0;
	//uint16_t adc1;
	uint8_t bytesToReceive = 1;
	char receiveBuffer[20]; // our reception buffer
	uint16_t compteur=0;
	uint16_t distance=0;
	/*DDRA |= ADC0; // output
	DDRA &= ~ADC1; // input
	PORTA &= ~ADC0; // Bas*/
	
	DDRC |= IO_PC5;
	DDRC &= ~IO_PC4;
	PORTC &= ~IO_PC5;

	//PORTA |= ADC0; // High
	//PORTA &= ~ADC0; // Bas
	writeString_P("\nDebut Programme NEW\n");

	// ---------------------------------------
	// Main loop:
	while(true)
	{
			writeString_P("\n***Debut du While***\n");

			//PORTA |= ADC0; // High
			PORTC |= IO_PC5;
			mSleep(1);
			//PORTA &= ~ADC0; // Bas
			PORTC &= ~IO_PC5;
			
			//adc1 = readADC(ADC_ADC1); // lit la donnée
			if(PINC & IO_PC4){              // Si on recoit un signal diff de 0
				writeString_P("*******\n");
				while(PINC & IO_PC4)       // tant que le signal recus est fidd de 0
				{
					compteur++;
					//mSleep(0.001);
					//adc1=readADC(ADC_ADC1);
				}
				distance=(compteur*10)/58;
				writeInteger(distance, DEC);
				compteur=0;
				distance=0;
				mSleep(1000);
			}

			/*receiveBytes(bytesToReceive); // Setup reception with number of bytes to receive
			waitUntilReceptionComplete(); // wait until everythig is received...
								  // This function waits FOREVER - it will not stop to
								  // wait for _all_ requested characters to be received!
			copyReceivedBytesToBuffer(&receiveBuffer[0]);

			//outputTextStuff();
			writeString_P("\n****");
			
			writeStringLength(&receiveBuffer[0],bytesToReceive,0); // Output the received data as a String
			writeString_P("***\n");*/

			
			
			//writeInteger(counter, BIN);

	}
	// End of main loop!
	// ---------------------------------------

	return 0;
}
