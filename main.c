/*
This is the code for the line following part of the bot. It follows the line just fine
and is capable of handling 90 degree turns. I'm not sure houw it will perform on turns that 
are acute angled. The code for handling the potentiometer turning need to be added. Also needed
is the code for ADC so that the target voltage on the metal strips can be read.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define GREEN 1
#define YELLOW 3
#define ORANGE 0
#define RED 2
#define ON 0xFF
#define OFF 0x00

//Connect INP A1 to PB0. New connection - OC0 - PB3
//Connect INP A2 to PB1. New connection - OC1B - PD4
//Connect INP B1 to PB2. New Connection - OC1A - PD5
//Connect INP B2 to PB3. New connection - OC2 - PD7
//Left motor is MOTORB
//Right motor is MOTORA
//Left motor Black wire goes to MB1
//Left motor Red wire goes to MB2
//Right motor red wire goes to MA1
//Right motor black wire goes to MA2
//Follow this wiring scheme and the turning instructions will be valid.
//The LEDs on the sensors go off when the surface is white.
//Connect the currently available sensor array to the first four pins of PORTC.

//Leftmost sensor is orange. And it goes to PINC2
//Second from the left is Green. And it goes to PINC0
//Third from the left is red. And it goes to PINC3 
//Fourth from the left is Yellow. and it goes to PINC1

//volatile int raw_reading=0;
volatile int sensor_reading=0;

void left()
{
	//Old code
	PORTB = 0b00001001;
}


void right()
{
	
	//Old code
	PORTB = 0b00000110;
}


void front()
{
	//Old code
	PORTB = 0b00000101;
}


void stop()
{

	//Old code
	PORTB=0b00000000;
}


void back()
{
	//Old code
	PORTB=0B00001010;
}


//void timer()
//{
//	TCCR1B |= 1<<CS12 | 1<<CS10 | 1<<WGM12;
//	TIMSK |= 1<<OCIE1A;
//	TCNT1 = 0;
//	OCR1A = 0;
//}



volatile int prev_reading=0;
volatile int flag = 0;
//volatile int time = 0;



int main()
{ 
	DDRC  = 0b11110000;
	DDRD  = 0b11111111;
	DDRB  = 0b11111111;
	PORTC = 0b00010000;
	PORTB = 0b00000000;
	
	//TCCR1A=0; TCCR1B=0; //Erasing previous settings.
	TCCR1A |= (1<<COM1A1)|(1<<COM1B1)|(1<<WGM10);
	OCR1A = ON; OCR1B = ON;//These are the enable A and enable B pins.
	TCCR1B |= (1<<CS12);
	
	//sei();
	//Leftmost sensor is orange. And it goes to PINC2
	//Second from the left is Green. And it goes to PINC0
	//Third from the left is red. And it goes to PINC3 
	//Fourth from the left is Yellow. and it goes to PINC1
	
	
	
	while(1)
	{	
		//PINC = 0b00011111;
		//The middle two bits are for the outer two sensors. The outer two bits are for the inner 2 sensors.
		//Getting the sensor reading from the pins.
		sensor_reading = PINC;
		
		//doing different stuff for different values of the sensor reading.
		switch(sensor_reading)
		{
			
			case (0b00011001): //The outer two sensors detect white.
				front();   //Go forward
				prev_reading = 1; //Set the counter to 1
				break;
			
			case (0b00011111): //All sensors are black
				//Go forward				
				front();   
				//Wait for a while
				_delay_ms(1);
				//Get sensor reading again
				sensor_reading = PINC;
				//increment the counter.
				if(sensor_reading == 0b00011111)
				{
					prev_reading++;
				}
				break;
	
			case (0b00011011): //Right three sensors detect line.
				right();
				prev_reading=1;
			        break;
			
			case (0b00010000): //All sensors are white. Either a discontinuity or the bot went off track.
				back();				
				break; 
	
			case (0b00011101): //Left three sensors detect a line.
				prev_reading = 1;
				left();					
				break;
			
			case (0b00010101)://Left two sensors detect a line.
				left();
				prev_reading = 3;
				break;
			
			case (0b00011010): //Right two sensors detect a line.
				right();
				prev_reading = 2;
				break;
			
			case (0b00010100): //Leftmost sensor detects a line.
				left();
				prev_reading = 3;
				break;
			
			case (0b00010010): //Rightmost sensor detects a line.
				right();
				prev_reading = 2;
				break;
						
			
			default:
				front(); //This is important. If this is not forward
				break;   //then the bot will stop randomly and annoy the hell out of you.
		}
		if(flag == 1||prev_reading>500) //If the sensors encounter all black for about half a second then it stops.
		{
			stop();
			break;
    	}
	}
	
	
	
}

