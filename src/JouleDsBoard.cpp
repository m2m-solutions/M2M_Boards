//---------------------------------------------------------------------------------------------
//
// Library for the Purplepoint Joule DS board.
//
// Copyright 2016-2020, M2M Solutions AB
// Written by Jonny Bergdahl, 2020-04-05
//
// Licensed under the MIT license, see the LICENSE.txt file.
//
//---------------------------------------------------------------------------------------------
//
#ifdef ARDUINO_PP_JOULE_DS
////////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
#include "JouleDsBoard.h"
#include "EEPROM.h"

#define SERIAL_NUMBER_ADDR		0
#define MQTT_PASSWORD_ADDR		SERIAL_NUMBER_ADDR + 24
#define ENCRYPTION_KEY_ADDR		PASSWORD_ADDR + 24
#define HASH_KEY_ADDR			ENCRYPRION_KEY_ADDR + 24
#define INVALID_TEMPERATURE		-1000.0

HertzBoard::HertzBoard()
    : watchdog()
{}

void HertzBoard::begin()
{
    pinMode(PIN_RFM_IRQ, INPUT);
    pinMode(PIN_RMF_RST, OUTPUT);
    pinMode(PIN_RF_CS, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_XDSHUT, OUTPUT);
    pinMode(PIN_PWR_AD_EN, OUTPUT);
	pinMode(PIN_TMP_AD_EN, OUTPUT);
	pinMode(PIN_WD_AWAKE, INPUT);
	pinMode(PIN_WD_DONE, OUTPUT);
    pinMode(PIN_PWR_AD, INPUT);
	pinMode(PIN_TMP_AD, INPUT);

	Wire.begin();
}

const void HertzBoard::getSerialNumber(char* buffer)
{
    CopyEepromString(SERIAL_NUMBER_ADDR, buffer);
}

const void HertzBoard::getMqttPassword(char* buffer)
{
    CopyEepromString(MQTT_PASSWORD_ADDR, buffer);
}

const void HertzBoard::getEncryptionKey(char* buffer)
{
    CopyEepromString(ENCRYPTION_KEY_ADDR, buffer);
}

const void HertzBoard::getHashKey(char* buffer)
{
    CopyEepromString(HASH_KEY_ADDR, buffer);
}

float HertzBoard::getTemperature()
{	
	digitalWrite(PIN_TMP_AD_EN, HIGH);
	delay(20);
	unit16_t value = analogRead(PIN_TMP_AD);
	digitalWrite(PIN_TMP_AD_EN, LOW);
}

float HertzBoard::getTemperatureInKelvin()
{
	float result = getTemperature();
	if (result != INVALID_TEMPERATURE)
	{
 		result += 273.15;		
	}
	return result;
}

float HertzBoard::getTemperatureInFarenheit()
{
	float result = getTemperature();
	if (result != INVALID_TEMPERATURE)
	{
 		result = result * 9.0f / 15.0f + 32.0f;		
	}
	return result;
}

float HertzBoard::getBatteryVoltage()
{
	digitalWrite(PIN_PWR_AD_EN, HIGH);	
	uint16_t value = 0;
	for (uint8_t i = 0; i < 10; i++)
	{
		delay(10);
	 	value += analogRead(PIN_PWR_AD);
	}
	return ((float)value / 3.3 * 1024 * 10);
}

void HertzBoard::setSwitchedPower(bool state)
{
	digitalWrite(PIN_VSW_EN, !state);
}

void HertzBoard::CopyEepromString(uint16_t address, char* buffer)
{
	uint8_t index = 0;
	unsigned char value;
	do
	{
		value = EEPROM[index];
    	buffer[index] = value;
		index++;
  	} while (value != 0);	
}

#endif