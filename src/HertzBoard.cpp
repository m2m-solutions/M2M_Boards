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
#ifdef ARDUINO_PP_HERTZ
////////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
#include "HertzBoard.h"
#include "Wire.h"
#include "util/rgbled.h"

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
    pinMode(FLASH_CS, OUTPUT);
    digitalWrite(FLASH_CS, HIGH);
    pinMode(MIRA_RESET, OUTPUT);
    digitalWrite(MIRA_RESET, HIGH);
    pinMode(RGB_LED, OUTPUT);
    digitalWrite(RGB_LED, HIGH);
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    pinMode(CM_RI, INPUT);
    pinMode(CM_STATUS, INPUT);

	Wire.begin();
}

const char* HertzBoard::getSerialNumber()
{
    return (char *)0x3FF3;
}

const char* HertzBoard::getEncryptionKey()
{
    return (char *)0x3FB0;
}

const char* HertzBoard::getHashKey()
{
    return (char *)0x3FD1;
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
    setRgbLed(RGB_LED, red, green, blue, _ledIntensity);
}

void HertzBoard::setLedIntensity(uint8_t percentage)
{
    _ledIntensity = percentage;
}

#endif