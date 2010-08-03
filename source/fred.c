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

#include <pic12f6x.h>
#include <pic.h>
//#include <htc.h>
//extern void eeprom_write(unsigned char addr, unsigned char value);
//extern unsigned char eeprom_read(unsigned char addr);

// ***************** Processor Init ****************************************
__IDLOC(1);
__CONFIG(INTIO & WDTDIS & MCLRDIS & BORDIS & UNPROTECT & PWRTEN);
__EEPROM_DATA(0xEA, 0x00, 0xC2, 0x01, 0xAA, 0x55, 0xBA, 0xDD);

#define EEPROM_SPEED_OFFSET 0
#define EEPROM_SPEED_MAX 2

// ********************* constants ***************************
#define MAX_SERVO_STATES 15
#define MODE_MAX_STATES 128
#define MODE_MIN_STATES 255

#define AN_START 		0x03 /* 0x0F */
#define SERVO			GPIO1
#define LED				GPIO5
#define TACH			GPIO2
#define BUTTON          GPIO3
#define xSERVO			0x02 /* GPIO1 */
#define xLED			0x20 /* GPIO5 */
#define xTACH			0x04 /* GPIO2 */
#define xBUTTON         0x08 /* GPIO3 */

#define T1_ON			0x05 /* prescale = 0 */
#define T1_OFF			0x04 

#define MODE_GET_MAX	0
#define MODE_GET_MIN	1
#define MODE_NO_PULSE	2
#define MODE_PULSE		3
#define MODE_TEST		4

//****************** variables *******************************
unsigned char servo_time;
unsigned char max_servo_states;
unsigned char servo_state;

unsigned int  speed_diff;
unsigned int  speed_max;
unsigned int  speed_scale;
unsigned int  speed_offset;
unsigned int  speed;
unsigned char ee_value;
unsigned char mode;
unsigned char tach_count;

bit speed_mult;
bit new_speed;

union 
{
    struct
    {
        unsigned char lower;
        unsigned char upper;
    } pieces;
    unsigned int value;
} t1, word;

// ****************** macros to set/clear GPIO *****************
// setting individual bits doesn't seem to work
#define GPIOSET(v) v = 1
#define GPIOCLR(v) v = 0


//***************************************************************************
unsigned char read_eeprom_byte(unsigned char addr)
{
	while(WR)
	{
		continue;
	}

	// use CARRY to remember state of Interrupt Enable
	CARRY=0;
	if(GIE)CARRY=1;
	GIE=0;

    EEADR=addr;
    RD=1;
    ee_value = EEDATA;

	// restore IE
	if(CARRY)GIE=1;

    return ee_value;
}
//***************************************************************************
void write_eeprom_byte(unsigned char addr, unsigned char value)
{
	while(WR)
	{
		continue;
	}
	
	// use CARRY to remember state of Interrupt Enable
	CARRY=0;
	if(GIE)CARRY=1;
	GIE=0;

	EEADR=addr;
	EEDATA=value;

	WREN=1;
	EECON2=0x55;
	EECON2=0xAA;
	WR=1;
	WREN=0;

	// restore IE
	if(CARRY)GIE=1;
}
//***************************************************************************
unsigned int read_word(unsigned char addr)
{
	word.pieces.lower = read_eeprom_byte(addr);
	word.pieces.upper = read_eeprom_byte(addr+1);
	
	return word.value;
}
//***************************************************************************
void write_word(unsigned char addr, unsigned int value)
{
	word.value = value;
	write_eeprom_byte(addr, word.pieces.lower);
	write_eeprom_byte(addr+1, word.pieces.upper);
}
//***************************************************************************
void compute_params()
{
	speed_diff = speed_max - speed_offset;
	if (speed_diff < 256) 
	{
		speed_scale = 256 / speed_diff;
		speed_mult = 1;
	} else {
		speed_scale = speed_diff / 256;
		speed_mult = 0;
	}
}
//***************************************************************************
//delay(value) - Delay Routine
//             - Delay=value*2.3ms (When OSC=4MHZ)
//***************************************************************************
unsigned char outer, inner;
void delay(char value)                    
{
	for (outer=value; outer != 0; outer--)	 
	{
		for (inner=0xFF; inner != 0; inner--)
		{
	  
		}
	}
	return;
}
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
	
	TRISIO = 0xFF & ~(xLED | xSERVO);
	//************* Init Done *******************
	delay(100);
	speed_offset = read_word(EEPROM_SPEED_OFFSET);
	speed_max = read_word(EEPROM_SPEED_MAX);
	if (speed_offset != 0xFFFF && speed_max != 0xFFFF)
	{
		compute_params();
		mode = MODE_PULSE;
		max_servo_states = MAX_SERVO_STATES;
	} else {
		mode = MODE_GET_MAX;
		max_servo_states = MODE_MAX_STATES;
	}	

	new_speed = 0;
	
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
				
				if (BUTTON)
				{
					if (new_speed)
					{
						if (speed_offset == 0) {
							speed_offset = speed;
						} else {
							speed_offset += speed;
							speed_offset /= 2;
						}
						new_speed = 0;
					}
				} else {
					new_speed = 0;
					if (speed_offset != 0) 
					{
						mode = MODE_GET_MIN;
						max_servo_states = MODE_MIN_STATES;
						delay(100);
					}
				}
				break;
			case MODE_GET_MIN:
				if (servo_state > MODE_MIN_STATES/4)
				{
					GPIOCLR(LED);
				} else {
					GPIOSET(LED);
				}
				
				if (BUTTON)
				{
					if (new_speed)
					{
						if (speed_max == 0) {
							speed_max = speed;
						} else {
							speed_max += speed;
							speed_max /= 2;
						}
						new_speed = 0;
					}
				} else {
					new_speed = 0;
					if (speed_max != 0) 
					{
						write_word(EEPROM_SPEED_OFFSET, speed_offset);
						write_word(EEPROM_SPEED_MAX, speed_max);
						mode = MODE_PULSE;
						max_servo_states = MAX_SERVO_STATES;
						compute_params();
						delay(100);
					}
				}
				break;
			case MODE_PULSE:
				if (new_speed)
				{
					if (speed < speed_offset)
					{
					      speed = speed_offset;
					}
					
					if (speed > speed_max+speed_offset)
					{
						speed = speed_max+speed_offset;
					}
			
					speed -= speed_offset;
					
					if (speed_mult)
					{
						speed *= speed_scale;
					} else {
						speed /= speed_scale;
					}
			
					if (speed > 255) speed = 255;
					servo_time = speed;
					
					new_speed = 0;
				} else if (TMR1H & 0x80) {
					mode = MODE_NO_PULSE;
					tach_count = 0;
				}
				break;
			case MODE_NO_PULSE:
				if (new_speed)
				{
					new_speed = 0;
					tach_count++;
					if (tach_count > 200) mode = MODE_PULSE;
				} else if (TMR1H & 0x80) {
					tach_count = 0;
				}
				break;
			case MODE_TEST:
				if (TMR1H & 0x01)
				{
					LED = 1;
				} else {
					LED = 0;
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
 
//***************************************************************************
//Isr() - Interrupt Service Routine
//      - Services Timer0 Overflow
//      - Services INT - external interrupt pin interrupt
//***************************************************************************
void interrupt Isr()
{
	if (T0IF)
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

		T0IF = 0;
	}

	if (INTF)
	{
		// Stop timer, get value, restart timer
		T1CON = T1_OFF;

	    t1.pieces.lower = TMR1L;
	    t1.pieces.upper = TMR1H;
		TMR1L = 0;
		TMR1H = 0;
    	T1CON = T1_ON;
		
		if (!new_speed)
		{		
			speed = t1.value;
			new_speed = 1;
		}
		
		INTF = 0;
	}

	return;
}
