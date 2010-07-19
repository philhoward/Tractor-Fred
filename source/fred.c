/*
The MIT License

Copyright (c) 2010 Philip W. Howard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <pic.h>
#include <pic12f6x.h>

// ***************** Processor Init ****************************************
__IDLOC(1);
__CONFIG(INTIO & WDTDIS & MCLRDIS & BORDIS & UNPROTECT & PWRTEN);

// ********************* constants ***************************
#define MAX_SERVO_STATES 15
#define MODE_MAX_STATES 128
#define MODE_MIN_STATES 255

#define AN_START 		0x03 /* 0x0F */
#define SERVO			0x02 /* GPIO1 */
#define LED				0x20 /* GPIO5 */
#define TACH			0x04 /* GPIO2 */
#define BUTTON          0x08 /* GPIO3 */
#define T1_THRESH		200
#define T1_ON			0x05 /* prescale = 0 */
#define T1_OFF			0x04 

#define MODE_GET_MAX	0
#define MODE_GET_MIN	1
#define MODE_NO_PULSE	2
#define MODE_PULSE		3

//****************** variables *******************************
unsigned char servo_time;
unsigned char gpio_state;
unsigned char value;
unsigned char servo_state;
unsigned char ledon;
unsigned int  max_speed;
unsigned int  min_speed;
unsigned int  speed_diff;
unsigned int  speed_max;
unsigned int speed_scale;
unsigned int speed_offset;
unsigned int speed;
unsigned char mode;
unsigned char max_servo_states;
bit new_time;
bit speed_mult;

union
{
    struct
    {
        unsigned char upper;
        unsigned char lower;
    } pieces;
    unsigned int value;
} t1;

// ****************** macros to set/clear GPIO *****************
// setting individual bits doesn't seem to work
#define GPIOSET(v) gpio_state|=v; GPIO=gpio_state
#define GPIOCLR(v) gpio_state&=(~v); GPIO=gpio_state

//***************************************************************************
//Main() - Main Routine
//***************************************************************************
void main()
{
	//************* Init *******************
	TRISIO = 0xFF;       				//Set All I/O's As Inputs
	GPIO = 0;                        	//Clear GPIO
	VRCON = 0;                       	//Turn Off Voltage Reference Peripheral
	CMCON = 0x07;                       //Turn Off Comparator Peripheral

	//WPU = 0x04;						// pull up RA2
	OPTION = 0xC1;       				// int edge on rising;
	                                    // pull ups disabled, prescaler = 1
	ANSEL = 0x33;						//RA0, RA2 Analog Input
	ADCON0 = AN_START;
	
	TMR0 = 0;                        	//Clear Timer0
	T0IF = 0;                        	//Clear Timer0 Overflow Interrupt Flag
	T0IE = 1;                          	//Timer0 Overflow Interrupt Enabled

	TMR1L = 0;
	TMR1H = 0;
	T1CON = T1_ON;						// Timer1 enabled, PS=0
	//TMR1IE = 1;

	INTF = 0;							// clear interrupt flag
	INTE = 1;							// external interrupt enable
	PEIE = 1;
	//GPIE = 1;
	GIE = 1;
	
	TRISIO = 0xFF & ~(LED | SERVO);
	//************* Init Done *******************
	mode = MODE_GET_MAX;
	max_servo_states = MODE_MAX_STATES;
		
	while(1)                            //Loop Forever
	{
		switch (mode)
		{
			case MODE_GET_MAX:
				if (servo_state > MODE_MAX_STATES/4)
				{
					GPIOCLR(LED);
				} else {
					GPIOSET(LED);
				}
				
				value = GPIO;
				if (value & BUTTON)
				{
					if (new_time)
					{
						if (max_speed == 0) {
							max_speed = t1.value;
						} else {
							max_speed += t1.value;
							max_speed /= 2;
						}
						new_time = 0;
					}
				} else if (max_speed != 0) {
					mode = MODE_GET_MIN;
					max_servo_states = MODE_MIN_STATES;
				}
				break;
			case MODE_GET_MIN:
				if (servo_state > MODE_MIN_STATES/4)
				{
					GPIOCLR(LED);
				} else {
					GPIOSET(LED);
				}
				
				value = GPIO;
				if (value & BUTTON)
				{
					if (new_time)
					{
						if (min_speed == 0) {
							min_speed = t1.value;
						} else {
							min_speed += t1.value;
							min_speed /= 2;
						}
						new_time = 0;
					}
				} else if (min_speed != 0) {
					mode = MODE_PULSE;
					max_servo_states = MAX_SERVO_STATES;
					speed_offset = max_speed;
					speed_max = min_speed;
					speed_diff = min_speed - max_speed;
					if (speed_diff < 256) 
					{
						speed_scale = 256/speed_diff;
						speed_mult = 1;
					} else {
						speed_scale = speed_diff/256;
						speed_mult = 0;
					}
				}
				break;
		}
		
		if ((ADCON0 & 0x02) == 0)
		{
			if (mode != MODE_PULSE)
			{
				servo_time = ADRESH;
			}
			ADCON0 = AN_START;
		} 
	}
}

void read_and_clear_t1()
{
	T1CON = T1_OFF;
    t1.pieces.lower = TMR1L;
    t1.pieces.upper = TMR1H;
	TMR1L = 0;
	TMR1H = 0;
    T1CON = T1_ON;
    new_time = 1;
}
  
//***************************************************************************
//Isr() - Interrupt Service Routine
//      - Services Timer0 Overflow
//      - Services GP3 Pin Change
//***************************************************************************
void interrupt Isr()
{
	if (T0IF)			  // T0IF //If A Timer0 Interrupt,  Then
	{
	    servo_state++;
	    if (servo_state >= max_servo_states) 
	    {
	    	servo_state = 0;
	    	GPIOSET(SERVO);
	    }
	    
	    else if (servo_state == 1)
	    {
	        TMR0 = servo_time;
	    }
	    
	    else if (servo_state == 2) 
	    {
	        GPIOCLR(SERVO);
	    }

		T0IF = 0;                     //Clear Timer0 Interrupt Flag
	}

	if (INTF)
	{
		read_and_clear_t1();
		speed = t1.value;
		if (speed > speed_max)
		{
			speed = speed_max;
		}
		
		if (speed < speed_offset)
		{
			speed = 0;
		} else {
			speed -= speed_offset;
		}
		
		if (speed_mult)
		{
			speed *= speed_scale;
		} else {
			speed /= speed_scale;
		}

		servo_time = speed;
		
		if (ledon)
		{
		    GPIOCLR(LED);
		    ledon = 0;
		} else {
		   	GPIOSET(LED);
		   	ledon = 1;
		}

		INTF = 0;
	}

	return;
}
