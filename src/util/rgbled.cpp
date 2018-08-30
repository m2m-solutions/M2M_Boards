//---------------------------------------------------------------------------------------------
//
// Library for the SK6812/WS2812B serial RGB LED types.
//
// Copyright 2018, M2M Solutions AB
// Written by Jonny Bergdahl, 2018-07-06
//
// Licensed under the MIT license, see the LICENSE.txt file.
//
//---------------------------------------------------------------------------------------------

#include <Arduino.h>

void setRgbLed(uint8_t pin, uint8_t red, uint8_t green, uint8_t blue, uint8_t intensity)
{
    uint8_t portNum = g_APinDescription[pin].ulPort;
    uint32_t  pinMask = 1ul << g_APinDescription[pin].ulPin;
    uint32_t bitMask = 0x800000;
	volatile uint32_t* set = &(PORT->Group[portNum].OUTSET.reg);
	volatile uint32_t* clr = &(PORT->Group[portNum].OUTCLR.reg);

    // Neopixels is ordered GRB
    uint32_t rgbValue = (green * intensity / 100) << 16;
    rgbValue |= (red * intensity / 100) << 8;
    rgbValue |= (blue * intensity / 100);

    noInterrupts(); //disable all interrupts
    for(int i = 0; i < 24; i++) {
		*set = pinMask;
        asm("nop; nop; nop; nop; nop; nop; nop; nop;");
        if(rgbValue & bitMask) {
            asm("nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop;");
            *clr = pinMask;
        } else {
            *clr = pinMask;
            asm("nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop;");
        }
        if(bitMask >>= 1) {
            asm("nop; nop; nop; nop; nop; nop; nop; nop; nop;");
        }
    }    	
    interrupts(); //enable all interrupts
}