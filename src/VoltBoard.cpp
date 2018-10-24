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

VoltBoard::VoltBoard()
    : watchdog()
{}

void VoltBoard::begin()
{
    pinMode(VSW_EN, OUTPUT);
    digitalWrite(VSW_EN, HIGH);
    pinMode(FLASH_CE, OUTPUT);
    pinMode(MIRA_RESET, OUTPUT);
    digitalWrite(MIRA_RESET, HIGH);
    pinMode(RGB_LED, OUTPUT);
    digitalWrite(RGB_LED, LOW);
	Wire.begin();
}

const char* VoltBoard::getSerialNumber()
{
    return (char *)0x3FF3;
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
	uint16_t result;
	if (!read16bitRegister(LM75A_REGISTER_TEMP, result))
	{
		return LM75A_INVALID_TEMPERATURE;
	}
	return (float)result / 256.0f;
/*
	Wire.beginTransmission(LM75A_ADDRESS);
	Wire.write(LM75A_REGISTER_TEMP);
	uint16_t result = Wire.endTransmission(true);
	if (result != 0)
	{
		SerialUSB.println("Cannot send");
		return LM75A_INVALID_TEMPERATURE;
	} 
	result = Wire.requestFrom(LM75A_ADDRESS, (uint8_t)2);
	if (result != 2)
	{
		SerialUSB.println("Cannot receive");
		return LM75A_INVALID_TEMPERATURE;
	}
	uint16_t response = Wire.read() << 8;
	response |= Wire.read();
	return (response / 256);
	*/
}

float VoltBoard::getTemperatureInKelvin()
{
	float result = getTemperature();
	if (result != LM75A_INVALID_TEMPERATURE)
	{
 		result += 273.15f;		
	}
	return result;
}

float VoltBoard::getTemperatureInFarenheit()
{
	float result = getTemperature();
	if (result != LM75A_INVALID_TEMPERATURE)
	{
 		result = result * 9.0f / 15.f + 32.f;		
	}
	return result;
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

uint32_t VoltBoard::getFreeMem()
{
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

bool VoltBoard::read16bitRegister(const uint8_t reg, uint16_t& response)
{
	uint8_t result;

	Wire.beginTransmission(LM75A_ADDRESS);
	Wire.write(reg);
	result = Wire.endTransmission();
	// result is 0-4 
	if (result != 0)
	{
		return false;
	}

	result = Wire.requestFrom(LM75A_ADDRESS, (uint8_t)2);
	if (result != 2)
	{
		return false;
	}
	uint8_t part1 = Wire.read();
	uint8_t part2 = Wire.read();
	
	//response = (Wire.read() << 8) | Wire.read();
	uint16_t temp = part1 << 8 | part2;
	response = part1 << 8 | part2;
	return true;
}

#endif