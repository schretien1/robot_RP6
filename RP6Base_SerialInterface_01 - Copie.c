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
								
								
/*******************************************/
								
#define IDLE  0

// The behaviour command data type:
typedef struct {
	uint8_t  speed_left;  // left speed (is used for rotation and 
						  // move distance commands - if these commands are 
						  // active, speed_right is ignored!)
	uint8_t  speed_right; // right speed
	unsigned dir:2;       // direction (FWD, BWD, LEFT, RIGHT)
	unsigned move:1;      // move flag
	unsigned rotate:1;    // rotate flag
	uint16_t move_value;  // move value is used for distance and angle values
	uint8_t  state;       // state of the behaviour
} behaviour_command_t;

/**********************************************/

#define CRUISE_SPEED_FWD    100 // Default speed when no obstacles are detected!

#define MOVE_FORWARDS 1
behaviour_command_t cruise = {CRUISE_SPEED_FWD, CRUISE_SPEED_FWD, FWD, 
								false, false, 0, MOVE_FORWARDS};

/******************************************/

#define AVOID_SPEED_L_ARC_LEFT  30
#define AVOID_SPEED_L_ARC_RIGHT 90
#define AVOID_SPEED_R_ARC_LEFT  90
#define AVOID_SPEED_R_ARC_RIGHT 30
#define AVOID_SPEED_ROTATE 	60

// States for the Avoid FSM:
#define AVOID_OBSTACLE_RIGHT 		1
#define AVOID_OBSTACLE_LEFT 		2
#define AVOID_OBSTACLE_MIDDLE	    3
#define AVOID_OBSTACLE_MIDDLE_WAIT 	4
#define AVOID_END 					5
behaviour_command_t avoid = {0, 0, FWD, false, false, 0, IDLE};


/*******************************/

behaviour_command_t STOP = {0, 0, FWD, false, false, 0, IDLE};
	uint16_t counter = 0;
	uint16_t adc0;
	uint16_t adc1;
	uint8_t bytesToReceive = 1;
	char receiveBuffer[20]; // our reception buffer
	uint16_t compteur=0;
	uint16_t distance=0;
	uint16_t var = 0;
/*****************************************************************************/

void behaviour_cruise(void)
{
}
void behaviourController(void)
{
    // Call all the behaviour tasks:
	behaviour_cruise();
	//behaviour_avoid();
	behaviour_escape();

    // Execute the commands depending on priority levels:
	if(escape.state != IDLE) // Highest priority - 3
		moveCommand(&escape);
	else if(avoid.state != IDLE) // Priority - 2
		moveCommand(&avoid);
	else if(cruise.state != IDLE) // Priority - 1
		moveCommand(&cruise); 
	else                     // Lowest priority - 0
		moveCommand(&STOP);  // Default command - do nothing! 
							 // In the current implementation this never 
							 // happens.
}

void moveCommand(behaviour_command_t * cmd)
{
	if(cmd->move_value > 0)  // move or rotate?
	{
		if(cmd->rotate)
			rotate(cmd->speed_left, cmd->dir, cmd->move_value, false); 
		else if(cmd->move)
			move(cmd->speed_left, cmd->dir, DIST_MM(cmd->move_value), false); 
		cmd->move_value = 0; // clear move value - the move commands are only
		                     // given once and then runs in background.
	}
	else if(!(cmd->move || cmd->rotate)) // just move at speed? 
	{
		changeDirection(cmd->dir);
		moveAtSpeed(cmd->speed_left,cmd->speed_right);
	}
	else if(isMovementComplete()) // movement complete? --> clear flags!
	{
		cmd->rotate = false;
		cmd->move = false;
	}
}

void acsStateChanged(void)
{
	if(obstacle_left && obstacle_right)
		statusLEDs.byte = 0b100100;
	else
		statusLEDs.byte = 0b000000;
	statusLEDs.LED5 = obstacle_left;
	statusLEDs.LED4 = (!obstacle_left);
	statusLEDs.LED2 = obstacle_right;
	statusLEDs.LED1 = (!obstacle_right);
	updateStatusLEDs();
}

void behaviour_escape(void)
{
	static uint8_t bump_count = 0;
	
	switch(escape.state)
	{
		case IDLE: 
		break;
		case ESCAPE_FRONT:
			escape.speed_left = ESCAPE_SPEED_BWD;
			escape.dir = BWD;
			escape.move = true;
			if(bump_count > 3)
				escape.move_value = 220;
			else
				escape.move_value = 160;
			escape.state = ESCAPE_FRONT_WAIT;
			bump_count+=2;
		break;
		case ESCAPE_FRONT_WAIT:			
			if(!escape.move) // Wait for movement to be completed
			{	
				escape.speed_left = ESCAPE_SPEED_ROTATE;
				if(bump_count > 3)
				{
					escape.move_value = 100;
					escape.dir = RIGHT;
					bump_count = 0;
				}
				else 
				{
					escape.dir = LEFT;
					escape.move_value = 70;
				}
				escape.rotate = true;
				escape.state = ESCAPE_WAIT_END;
			}
		break;
		case ESCAPE_LEFT:
			escape.speed_left = ESCAPE_SPEED_BWD;
			escape.dir 	= BWD;
			escape.move = true;
			if(bump_count > 3)
				escape.move_value = 190;
			else
				escape.move_value = 150;
			escape.state = ESCAPE_LEFT_WAIT;
			bump_count++;
		break;
		case ESCAPE_LEFT_WAIT:
			if(!escape.move) // Wait for movement to be completed
			{
				escape.speed_left = ESCAPE_SPEED_ROTATE;
				escape.dir = RIGHT;
				escape.rotate = true;
				if(bump_count > 3)
				{
					escape.move_value = 110;
					bump_count = 0;
				}
				else
					escape.move_value = 80;
				escape.state = ESCAPE_WAIT_END;
			}
		break;
		case ESCAPE_RIGHT:	
			escape.speed_left = ESCAPE_SPEED_BWD;
			escape.dir 	= BWD;
			escape.move = true;
			if(bump_count > 3)
				escape.move_value = 190;
			else
				escape.move_value = 150;
			escape.state = ESCAPE_RIGHT_WAIT;
			bump_count++;
		break;
		case ESCAPE_RIGHT_WAIT:
			if(!escape.move) // Wait for movement to be completed
			{ 
				escape.speed_left = ESCAPE_SPEED_ROTATE;		
				escape.dir = LEFT;
				escape.rotate = true;
				if(bump_count > 3)
				{
					escape.move_value = 110;
					bump_count = 0;
				}
				else
					escape.move_value = 80;
				escape.state = ESCAPE_WAIT_END;
			}
		break;
		case ESCAPE_WAIT_END:
			if(!(escape.move || escape.rotate)) // Wait for movement/rotation to be completed
				escape.state = IDLE;
		break;
	}
}


/***********************************************************/
// Main function - The program starts here:

int main(void)
{
	initRobotBase();// Always call this first! The Processor will not work
					 // correctly otherwise.
	powerON();
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
										
					stopStopwatch4();
					setStopwatch4(0);
					//avoid.state = IDLE;
					//mSleep(2000);
					//mSleep(2000);
				}
				else
				{
					setLEDs(0b000000);	
					behaviourController();

				}
					
			}
			
	}

	return 0;
}
