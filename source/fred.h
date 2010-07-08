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

#define	PUSHBUTTON	 	GPIO3		//GP3 - Pushbutton Input
#define	SET				1
#define	CLEAR			0

#define AN_START 		0x0F

//Global Variable Declarations
unsigned char ontime;
unsigned char offtime;
unsigned char led1on;
unsigned char gpio_state;
unsigned char value;
#define MAX_SERVO_STATE 15
unsigned char servo_state;

#define GPIOSET(v) gpio_state|=v; GPIO=gpio_state
#define GPIOCLR(v) gpio_state&=v; GPIO=gpio_state

#endif
