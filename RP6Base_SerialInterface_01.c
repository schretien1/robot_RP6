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
								// Always needs to be included!
								
	uint16_t adc0;
	uint16_t adc1;
	uint8_t bytesToReceive = 1;
	char receiveBuffer[20]; // our reception buffer
	uint16_t compteur=0;
	uint16_t distance=0;
	uint16_t var = 0;								
/***********************************************************/

void bumpersStateChanged(void)
{
	if(bumper_left || bumper_right) 
	{
		moveAtSpeed(0,0);  // stop moving!
		setLEDs(0b010000);
		startStopwatch1();
	}
}

/***************************/
/*
void blink(void)
{
	if(getStopwatch1() > 500) // 500ms
	{
		statusLEDs.LED2 = !statusLEDs.LED2; // Toggle LED bit in LED shadow register... 
		statusLEDs.LED5 = !statusLEDs.LED5;
		updateStatusLEDs();
		setStopwatch1(0);
	}
}
*/
/***************************************************/

// Main function - The program starts here:

int main(void)
{
	initRobotBase();// Always call this first! The Processor will not work
					 // correctly otherwise.
					 
	BUMPERS_setStateChangedHandler(bumpersStateChanged);
					 
	powerON();
				
	changeDirection(FWD);
	moveAtSpeed(90,90);  

			
	DDRA |= ADC0; // output
	DDRA &= ~ADC1; // input
	PORTA &= ~ADC0; // Bas				 
	//setLEDs(0b111111); // Turn all LEDs on
	//mSleep(500);       // delay 500ms
	//setLEDs(0b000000); // All LEDs off

	// Write a text message to the UART:
	writeString_P("\nJust a simple counter program\n\n");
	
	// Define a counting variable:


	//PORTA |= ADC0; // High
	//PORTA &= ~ADC0; // Bas
	writeString_P("\nDebut Programme\n");

	// ---------------------------------------
	// Main loop:
	while(true)
	{
		
		//blink();					
		task_RP6System();
				
			PORTA |= ADC0; // High
			mSleep(1);
			PORTA &= ~ADC0; // Bas
			
			compteur=0;
			distance=0;
			adc1 = readADC(ADC_ADC1); // lit la donnée
			
			if(adc1!=0){              // Si on recoit un signal diff de 0
				while(adc1!=0)       // tant que le signal recus est fidd de 0
				{
					compteur++;
					mSleep(0.001);
					//uSleep(10);
					adc1=readADC(ADC_ADC1);
				}
				distance=(compteur*100)/58;
				writeInteger(distance, DEC);
				writeString_P("\n");
				//mSleep(1000);
				
				//Test si la valeur est exacte
				//Si on est inferieur a 8 on incrémente "var" 
				if(distance<=8 && var<=50)
				{
					var++;
				}
				
				// Si var est superieur a 0 on decremente pour pas depasser le max 100
				else if(var>0)
					var--;
				
				//Si la valeur est bien superieur a 8 on allume la LED
				if(var>=8)
				{

					setLEDs(0b001001);
					moveAtSpeed(0,0);  

					//avoid.state = IDLE;
					//mSleep(2000);
					//mSleep(2000);
				}
				else
				{
					setLEDs(0b000000);		
				}
			
			}
			
	}

	return 0;
}
