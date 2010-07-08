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

#include "fred.h"

//***************************************************************************
//Main() - Main Routine
//***************************************************************************
void main()
{
	//************* Init *******************
	TRISIO = 0xFF;                       //Set All I/O's As Inputs
	GPIO = CLEAR;                        //Clear GPIO
	VRCON = CLEAR;                       //Turn Off Voltage Reference Peripheral
	CMCON = 0x07;                        //Turn Off Comparator Peripheral

	//WPU = 0x04;							// pull up RA2
	OPTION = 0xC1;       				// int edge on rising;
	                                    // pull ups disabled, prescaler = 1
	ANSEL = 0x39;						 //RA4 Analog Input
	ADCON0 = AN_START;
	
	TMR0 = CLEAR;                        //Clear Timer0
	T0IF = CLEAR;                        //Clear Timer0 Overflow Interrupt Flag
	T0IE = SET;                          //Timer0 Overflow Interrupt Enabled
	TMR1L = 0;
	TMR1H = 0;
	T1CON = 0x35;						// Timer1 enabled, PS=3
	//TMR1IE = 1;
	//IOC = 0x04;							// interrupt on RA2 change
	INTF = 0;							// clear interrupt flag
	//INTE = 1;							// external interrupt enable
	PEIE = 1;
	GIE = 1;
	
	TRISIO = 0xDD;						// RA0 is input for POT; 
	                                    // RA1 is output for servo
	                                    // RA5 is output for debut LED
	//************* Init Done *******************
		
	while(1)                            //Loop Forever
	{	
		if (servo_state == 1)
		{
			if (TMR0 >= ontime) GPIOCLR(0xFD);
		} 
					
		if ((ADCON0 & 0x02) == 0)
		{
			ontime = ADRESH;
			offtime = 255-ontime;
			ADCON0 = AN_START;
		} 

		value = GPIO;		
		if (value & 0x04)
		{
			GPIOSET(0x20);
		} else {
		    GPIOCLR(0xDF);
		}
	}
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
	    if (servo_state >= MAX_SERVO_STATE) 
	    {
	    	servo_state = 0;
	    	GPIOSET(2);
	    }
	    else 
	    {
		    servo_state++;
		    if (servo_state == 2) GPIOCLR(0xFD);
		}
		
		T0IF = 0;                     //Clear Timer0 Interrupt Flag
	}

	if (INTF)
	{
/*
		if (gpio5on)
		{
		    GPIO5 = 0;
		    gpio5on = 0;
		} else {
		    GPIO5 = 1;
		    gpio5on = 1;
		}
*/
		INTF = 0;
	}

	return;
}
