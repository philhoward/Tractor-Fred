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

#ifndef FRED_H
#define FRED_H

#include <pic.h>
#include <pic12f6x.h>

__IDLOC(1);
__CONFIG(INTIO & WDTDIS & MCLRDIS & BORDIS & UNPROTECT & PWRTEN);

//Defines

#define	LED1TRIS	0b11001111
#define	LED2TRIS	0b11001111
#define	LED3TRIS	0b11101011
#define	LED4TRIS	0b11101011
#define	LED5TRIS	0b11011011
#define	LED6TRIS	0b11011011
#define	LED7TRIS	0b11111001
#define	LED8TRIS	0b11111001
#define LED1ON		0b00010000
#define	LED2ON		0b00100000
#define	LED3ON		0b00010000
#define	LED4ON		0b00000100
#define	LED5ON		0b00100000
#define	LED6ON		0b00000100
#define	LED7ON		0b00000100
#define	LED8ON		0b00000010
#define	PUSHBUTTON	 	GPIO3		//GP3 - Pushbutton Input
#define	DEBOUNCEDELAY	200		    //Debounce Delay Constant
#define	OPEN			1
#define	CLOSED			0
#define	SET				1
#define	CLEAR			0
#define	FIRSTSTATE		0
#define LASTSTATE		7
#define	TRUE			1
#define	FALSE			0
#define FORWARD			0

#define AN_START 		0x0F

//Global Variable Declarations
unsigned char inner,outer;
unsigned char ontime;
unsigned char offtime;
unsigned char led1on;
unsigned char value;
#define MAX_SERVO_STATE 20
unsigned char servo_state;

//Function Prototypes
void Init();
void Led1();
void Led2();
void Led3();
void Led4();
void Led5();
void Led6();
void Led7();
void Led8();
void Delay(char value);
void Display();

#endif
