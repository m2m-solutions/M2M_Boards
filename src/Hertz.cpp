//---------------------------------------------------------------------------------------------
//
// Library for the Mira One radio module.
//
// Copyright 2016-2017, M2M Solutions AB
// Written by Jonny Bergdahl, 2017-05-09
//
// Licensed under the MIT license, see the LICENSE.txt file.
//
//---------------------------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
#include "Hertz.h"
#include "Wire.h"

#define LM75A_ADDRESS				0x48
#define LM75A_REGISTER_TEMP			0
#define LM75A_INVALID_TEMPERATURE	-1000.0f

HertzBoard::HertzBoard()
    : watchdog()
{}

void HertzBoard::begin()
{
    pinMode(CM_PWRKEY, OUTPUT);
    digitalWrite(CM_PWRKEY, LOW);
    pinMode(CM_PWR_EN, OUTPUT);
    digitalWrite(CM_PWR_EN, LOW);
    pinMode(FLASH_CE, OUTPUT);
    digitalWrite(CM_PWR_EN, LOW);
    pinMode(MIRA_RESET, OUTPUT);
    digitalWrite(CM_PWR_EN, LOW);
    pinMode(RGB_LED, OUTPUT);
    digitalWrite(RGB_LED, LOW);
    pinMode(SD_CS, OUTPUT);
    digitalWrite(CM_PWR_EN, LOW);
    pinMode(CM_RI, INPUT);
    pinMode(CM_STATUS, INPUT);

	Wire.begin();
}

float HertzBoard::getTemperature()
{	
	Wire.beginTransmission(LM75A_ADDRESS);
	Wire.write(LM75A_REGISTER_TEMP);
	uint16_t result = Wire.endTransmission();
	if (result != 0)
	{
		return LM75A_INVALID_TEMPERATURE;
	}
	result = Wire.requestFrom(LM75A_ADDRESS, (uint8_t)2);
	if (result != 2)
	{
		return false;
	}
	uint16_t response = Wire.read() << 8;
	response |= Wire.read();
	return response / 256;
}

void HertzBoard::setCellularPower(bool state)
{
    digitalWrite(CM_PWR_EN, state);
}

uint8_t HertzBoard::getCellularStatus()
{
	return !digitalRead(CM_STATUS);
}

void HertzBoard::setLed(uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t portNum = g_APinDescription[RGB_LED].ulPort;
    uint32_t  pinMask = 1ul << g_APinDescription[RGB_LED].ulPin;
    uint32_t bitMask = 0x800000;
	volatile uint32_t* set = &(PORT->Group[portNum].OUTSET.reg);
	volatile uint32_t* clr = &(PORT->Group[portNum].OUTCLR.reg);

    // Neopixels is ordered GRB
    uint32_t rgbValue = (green * _ledIntensity / 100) << 16;
    rgbValue |= (red * _ledIntensity / 100) << 8;
    rgbValue |= (blue * _ledIntensity / 100);

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
}

void HertzBoard::setLedIntensity(uint8_t percentage)
{
    _ledIntensity = percentage;
}

