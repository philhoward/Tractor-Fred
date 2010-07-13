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
#define MAX_SERVO_STATE 15
#define AN_START 		0x03 /* 0x0F */
#define SERVO			0x02 /* GPIO1 */
#define LED				0x20 /* GPIO5 */
#define TACH			0x08 /* GPIO3 */
#define DEBUG           0x04 /* GPIO2 */
#define T1_THRESH		200
#define T1_ON			0x05 /* prescale = 0 */
#define T1_OFF			0x04 

//****************** variables *******************************
unsigned char ontime;
unsigned char offtime;
unsigned char gpio_state;
unsigned char value;
unsigned char servo_state;
unsigned char gpio5on;

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
	TRISIO = 0xFF;                       //Set All I/O's As Inputs
	GPIO = 0;                        	//Clear GPIO
	VRCON = 0;                       	//Turn Off Voltage Reference Peripheral
	CMCON = 0x07;                        //Turn Off Comparator Peripheral

	//WPU = 0x04;							// pull up RA2
	OPTION = 0xC1;       				// int edge on rising;
	                                    // pull ups disabled, prescaler = 1
	ANSEL = 0x39;						 //RA0, RA3 Analog Input
	ADCON0 = AN_START;
	
	TMR0 = 0;                        	//Clear Timer0
	T0IF = 0;                        	//Clear Timer0 Overflow Interrupt Flag
	T0IE = 1;                          	//Timer0 Overflow Interrupt Enabled
	TMR1L = 0;
	TMR1H = 0;
	T1CON = T1_ON;						// Timer1 enabled, PS=0
	//TMR1IE = 1;
	//IOC = 0x04;						// interrupt on RA2 change
	IOCB3 = 1;
	//INTF = 0;							// clear interrupt flag
	//INTE = 1;							// external interrupt enable
	PEIE = 1;
	GPIE = 1;
	GIE = 1;
	
	TRISIO = 0xD9;						// //RA0 is input for POT; 
	                                    // RA1 is output for servo
	                                    // RA2 is output for debug
	                                    // RA3 is input for TACH
	                                    // RA4 is input for POT
	                                    // RA5 is output for debug LED
	//************* Init Done *******************
		
	while(1)                            //Loop Forever
	{
		// compiler bug: program doesn't work without this NOOP
		if (servo_state)
		{
		}
		
		if ((ADCON0 & 0x02) == 0)
		{
			ontime = ADRESH;
			offtime = 255-ontime;
			ADCON0 = AN_START;
		} 

	    value = TMR1H;
	    if (value & 0x01)
	    {
		    //if (gpio5on)
    		//{
			    GPIOCLR(DEBUG);
			    //gpio5on = 0;
			//}
		} else {//if (!gpio5on) {
		   	GPIOSET(DEBUG);
		   	//gpio5on = 1;
		}
	
	    if (t1.value > T1_THRESH)
    	{
		    GPIOCLR(LED);
		} else {
	    	GPIOSET(LED);
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
}
  
//***************************************************************************
//Isr() - Interrupt Service Routine
//      - Services Timer0 Overflow
//      - Services GP3 Pin Change
//***************************************************************************
void interrupt Isr()
{
	if (T0IF)			  //If A Timer0 Interrupt,  Then
	{
	    servo_state++;
	    if (servo_state >= MAX_SERVO_STATE) 
	    {
	    	servo_state = 0;
	    	GPIOSET(SERVO);
	    }
	    
	    else if (servo_state == 1)
	    {
	        TMR0 = ontime;
	    }
	    
	    else if (servo_state == 2) 
	    {
	        GPIOCLR(SERVO);
	    }

		T0IF = 0;                     //Clear Timer0 Interrupt Flag
	}

	if (GPIF)
	{
	
	    gpio_state = GPIO;
	    if (gpio_state & TACH)
	    {
	    	read_and_clear_t1();
		}
		
		GPIF = 0;
	}


	return;
}
