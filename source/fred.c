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

	OPTION = 0x85;       				// pull ups disabled, prescaler = 5
	ANSEL = 0x31;						 //RA0 Analog Input
	ADCON0 = 0x01;
	
	//IOCB3 = SET;                         //GP3 Interrupt On Pin Changed Enabled
	//GPIE = SET;                          //Interrupt On Pin Change Enabled
	TMR0 = CLEAR;                        //Clear Timer0
	T0IF = CLEAR;                        //Clear Timer0 Overflow Interrupt Flag
	T0IE = SET;                          //Timer0 Overflow Interrupt Enabled
	//ADIE = SET;
	//ADIF = CLEAR;
	//GPIF = CLEAR;                        //Clear Interrupt On Pin Change Flag
	//GIE = SET;                           //Enable All Interrupts
	//TMR1L = 0;
	//TMR1H = 0;
	
	//T1CON = 0x35;						// Timer1 enabled, PS=3
	//TMR1IE = 1;
	PEIE = 1;
	GIE = 1;
	//************* Init Done *******************
		
	while(1)                             //Loop Forever
	{	
		//Delay(ontime);
		//Led1();
		//Delay(offtime);
		//Led2();
		
		if (servo_state == 1)
		{
			if (TMR0 >= ontime) Led2();
		}
				
		/*
		if (TMR1IF)
		{
		    value++;
		    TMR1IF = 0;
		}
		*/
		//value = TMR1H;
		/*
		if (value & 0x01) Led1();
		if (value & 0x02) Led2();
		if (value & 0x04) Led3();
		if (value & 0x08) Led4();
		if (value & 0x10) Led5();
		if (value & 0x20) Led6();
		if (value & 0x40) Led7();
		if (value & 0x80) Led8();
		*/
		
		if ((ADCON0 & 0x02) == 0)
		{
			ontime = ADRESH;
			offtime = 255-ontime;
			ADCON0 = 0x03;
		} 
	}
}


//***************************************************************************
//Init - Initialization Routine
//***************************************************************************
void Init()
{
/*
	#asm 								 //Load Factory Calibration Value Into OSCCAL
		call 0x3FF
		bsf	_STATUS,5
		movwf _OSCCAL
	#endasm
*/	
	TRISIO = 0xFF;                       //Set All I/O's As Inputs
	GPIO = CLEAR;                        //Clear GPIO
	VRCON = CLEAR;                       //Turn Off Voltage Reference Peripheral
	CMCON = 0x07;                        //Turn Off Comparator Peripheral
	TMR0 = CLEAR;                        //Clear Timer0
/*
	OPTION = 0x85;       				// pull ups disabled, prescaler = 5
	ANSEL = 0x31;						 //RA0 Analog Input
	ADCON0 = 0x01;
*/	
	//IOCB3 = SET;                         //GP3 Interrupt On Pin Changed Enabled
	//GPIE = SET;                          //Interrupt On Pin Change Enabled
	T0IF = CLEAR;                        //Clear Timer0 Overflow Interrupt Flag
	//T0IE = SET;                          //Timer0 Overflow Interrupt Enabled
	//ADIE = SET;
	//ADIF = CLEAR;
	//GPIF = CLEAR;                        //Clear Interrupt On Pin Change Flag
	//GIE = SET;                           //Enable All Interrupts
	TMR1L = 0;
	TMR1H = 0;
	
	GIE = 1;
	TMR1IE = 1;
	PEIE = 1;
	T1CON = 0x35;						// Timer1 enabled, PS=3

	ontime = 0;
	offtime = 255;

	return;
}

//***************************************************************************
//Functions
//***************************************************************************

//***************************************************************************
//Led1 - D0
//***************************************************************************
void Led1()                              
{
	TRISIO = LED1TRIS;
	GPIO = LED1ON;
	return;
}

//***************************************************************************
//Led2 - D1
//***************************************************************************
void Led2()				 				 
{
	TRISIO = LED2TRIS;
	GPIO = LED2ON;
	return;
}
//***************************************************************************
//Led3 - D2
//***************************************************************************
void Led3()                              
{
	TRISIO = LED3TRIS;
	GPIO = LED3ON;
	return;
}

//***************************************************************************
//Led4 - D3
//***************************************************************************
void Led4()								 
{
	TRISIO = LED4TRIS;
	GPIO = LED4ON;
	return;
}

//***************************************************************************
//Led5 - D4
//***************************************************************************
void Led5()								 
{
	TRISIO = LED5TRIS;
	GPIO = LED5ON;
	return;
}

//***************************************************************************
//Led6 - D5
//***************************************************************************
void Led6()								 
{
	TRISIO = LED6TRIS;
	GPIO = LED6ON;
	return;
}

//***************************************************************************
//Led7() - D6
//***************************************************************************
void Led7()                              
{
	TRISIO = LED7TRIS;
	GPIO = LED7ON;
	return;
}
//***************************************************************************
//Led8() - D7
//***************************************************************************
void Led8()                              
{
	TRISIO = LED8TRIS;
	GPIO = LED8ON;
	return;
}

//***************************************************************************
//Delay(value) - Delay Routine
//             - Delay=value*2.3ms (When OSC=4MHZ)
//***************************************************************************
void Delay(char value)                    
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
//Isr() - Interrupt Service Routine
//      - Services Timer0 Overflow
//      - Services GP3 Pin Change
//***************************************************************************
void interrupt Isr()
{
	if (TMR1IF)
	{
	    if (led1on)
	    {
	        Led3();
	        led1on = 0;
	    } else {
	        Led1();
	        led1on = 1;
	    }
        TMR1IF = 0;
	}
	
	if (T0IF)			  //If A Timer0 Interrupt, Then
	{
	    if (servo_state >= MAX_SERVO_STATE) 
	    {
	    	servo_state = 0;
	    	Led1();
	    }
	    else
	    {
		    servo_state++;
		}
		
		T0IF = 0;                     //Clear Timer0 Interrupt Flag
	}
/*
	
	if ((GPIE & GPIF) == SET)					  //If A GP3 Pin-Change Interrupt
	{
		if (Debounce() == TRUE)     		 //Debounce Pushbutton
			leddirection ^= 0x1;
			      					     //Toggle Timer0 Prescaler (Changes LED Flashing Speed)
		GPIF = CLEAR;                    //Clear Interrupt On Pin Change Flag
	}
	
	if ((ADIE & ADIF) == SET)			//Binary search used to change timer0 prescaler
	{
									    //which is used for led speed control
		if (ADRESH < 0x7F)
		{
			if (ADRESH > 0x3F)
			{
				if (ADRESH > 0x5E)
					OPTION = 0x84;
				else				//ADRESH < 0X5E
					OPTION = 0x85;
			}
				
			else 					//ADRESH < 0X3F
			{	if (ADRESH > 0x1F)
					OPTION = 0x86;
				else 				//ADRESH < 0X1F
					OPTION = 0x87;
			}		
					
		}
		else  						//ADRESH > 0x7F
		{
			if (ADRESH > 0xBE)
			{
				if (ADRESH > 0xDD)
					OPTION = 0x80;	
				else				//ADRESH < 0XDD
					OPTION = 0x81;				
			}
			else				    //ADRESH < 0XBE
			{
				if (ADRESH > 0x9F)
					OPTION = 0x82;
				else				//ADRESH < 0X9F
					OPTION = 0x83;
			}	
		}

		delaytime = ADRESH;
		ADIF = CLEAR;				//Clear A/D Interrupt Flag
	}
*/
	return;
}
