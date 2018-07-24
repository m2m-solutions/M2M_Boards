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
#ifdef ARDUINO_PP_VOLT
////////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
#include "VoltBoard.h"
#include "Wire.h"
#include "util/rgbled.h"

#define LM75A_ADDRESS				0x48
#define LM75A_REGISTER_TEMP			0
#define LM75A_REGISTER_CONFIG		1
#define LM75A_INVALID_TEMPERATURE	-1000.0f

VoltBoard::VoltBoard()
    : watchdog()
{}

void VoltBoard::begin()
{
	Wire.begin();
    pinMode(VSW_EN, OUTPUT);
    digitalWrite(VSW_EN, HIGH);
    pinMode(FLASH_CE, OUTPUT);
    pinMode(MIRA_RESET, OUTPUT);
    digitalWrite(MIRA_RESET, HIGH);
    pinMode(RGB_LED, OUTPUT);
    digitalWrite(RGB_LED, LOW);
}

char * VoltBoard::getSerialNumber()
{
    return (char *)0x3FF0;
}

const char* VoltBoard::getEncryptionKey()
{
    return (char *)0x3FB0;
}

const char* VoltBoard::getHashKey()
{
    return (char *)0x3FD1;
}

float VoltBoard::getTemperature()
{
	Wire.beginTransmission(LM75A_ADDRESS);
	Wire.write(LM75A_REGISTER_TEMP);
	uint16_t result = Wire.endTransmission(true);
	if (result != 0)
	{
		return LM75A_INVALID_TEMPERATURE;
	} 
	result = Wire.requestFrom(LM75A_ADDRESS, (uint8_t)2);
	if (result != 2)
	{
		return LM75A_INVALID_TEMPERATURE;
	}
	uint16_t response = Wire.read() << 8;
	response |= Wire.read();
	return response / 256;
}

void VoltBoard::setSwitchedPower(bool state)
{
    digitalWrite(VSW_EN, !state);
}

void VoltBoard::setLed(uint8_t red, uint8_t green, uint8_t blue)
{
    setRgbLed(RGB_LED, red, green, blue, _ledIntensity);   
}

void VoltBoard::setLedIntensity(uint8_t percentage)
{
    _ledIntensity = percentage;
}

#endif