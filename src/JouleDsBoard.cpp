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
#include <EEPROM.h>
#include <Wire.h>

#define SERIAL_NUMBER_ADDR		0
#define MQTT_PASSWORD_ADDR		SERIAL_NUMBER_ADDR + 24
#define ENCRYPTION_KEY_ADDR		MQTT_PASSWORD_ADDR + 24
#define HASH_KEY_ADDR			ENCRYPTION_KEY_ADDR + 24
#define INVALID_TEMPERATURE		-1000.0

JouleDsBoard::JouleDsBoard()
    : watchdog()
{}

void JouleDsBoard::begin()
{
    pinMode(PIN_RFM_IRQ, INPUT);
    pinMode(PIN_RFM_RST, OUTPUT);
    pinMode(PIN_RFM_CS, OUTPUT);
	pinMode(PIN_VSW_EN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_DSXSHUT, OUTPUT);
    pinMode(PIN_PWR_AD_EN, OUTPUT);
	pinMode(PIN_TMP_AD_EN, OUTPUT);
	pinMode(PIN_WD_AWAKE, INPUT);
	pinMode(PIN_WD_DONE, OUTPUT);
    pinMode(PIN_PWR_AD, INPUT);
	pinMode(PIN_TMP_AD, INPUT);

	Wire.begin();
}

void JouleDsBoard::getSerialNumber(char* buffer)
{
    CopyEepromString(SERIAL_NUMBER_ADDR, buffer);
}

void JouleDsBoard::getMqttPassword(char* buffer)
{
    CopyEepromString(MQTT_PASSWORD_ADDR, buffer);
}

void JouleDsBoard::getEncryptionKey(char* buffer)
{
    CopyEepromString(ENCRYPTION_KEY_ADDR, buffer);
}

void JouleDsBoard::getHashKey(char* buffer)
{
    CopyEepromString(HASH_KEY_ADDR, buffer);
}

float JouleDsBoard::getTemperature()
{	
	digitalWrite(PIN_TMP_AD_EN, HIGH);
	delay(20);
	uint16_t value = 1023 - analogRead(PIN_TMP_AD);
	digitalWrite(PIN_TMP_AD_EN, LOW);
	//return (float)value;

	float R0 = 5100;
	float T0=298.15;   // use T0 in Kelvin [K]
	float T1=273.15;      // [K] in datasheet 0º C
	float T2=373.15;      // [K] in datasheet 100° C
	float RT1=272186;   // [ohms]  resistence in T1
	float RT2=4.3008;    // [ohms]   resistence in T2
	float beta = (log(RT1 / RT2)) / ((1 / T1) - (1 / T2));
	float Rinf = R0 * exp(-beta / T0);

	float vcc = getRegulatedVoltage();
	float vout = vcc * ((float)value / 1024.0); // calc for ntc
  	float rout = (10000 * vout / (vcc - vout));
	
  	float TempK=(beta/log(rout/Rinf)); // calc for temperature
  	return TempK - 273.15;
}

float JouleDsBoard::getTemperatureInKelvin()
{
	float result = getTemperature();
	if (result != INVALID_TEMPERATURE)
	{
 		result += 273.15;		
	}
	return result;
}

float JouleDsBoard::getTemperatureInFarenheit()
{
	float result = getTemperature();
	if (result != INVALID_TEMPERATURE)
	{
 		result = result * 9.0f / 15.0f + 32.0f;		
	}
	return result;
}

float JouleDsBoard::getBatteryVoltage()
{
	digitalWrite(PIN_PWR_AD_EN, HIGH);	
	delay(20);	// We have 100nF cap on the ADC input, just let let it charge
	uint16_t value = analogRead(PIN_PWR_AD);
	digitalWrite(PIN_PWR_AD_EN, LOW);
	float vcc = getRegulatedVoltage();
	float percentage = 2 * value / 1024.0;
	return (percentage * vcc);
}

float JouleDsBoard::getRegulatedVoltage()
{
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);

	delay(2); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA,ADSC)); // measuring

	uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
	uint8_t high = ADCH; // unlocks both

	long result = (high<<8) | low;

	result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	return result / 1000.0; // Vcc in millivolts
}

void JouleDsBoard::setSwitchedPower(bool state)
{
	digitalWrite(PIN_VSW_EN, !state);
}

void JouleDsBoard::setDistanceSensorEnable(bool state)
{
	digitalWrite(PIN_DSXSHUT, state);
}

void JouleDsBoard::setLed(bool state)
{
	digitalWrite(LED_BUILTIN, state);
}

void JouleDsBoard::CopyEepromString(uint16_t address, char* buffer)
{
	uint16_t index = address;
	unsigned char value;
	do
	{
		value = EEPROM[index];
    	buffer[index] = value;
		index++;
  	} while (value != 0);	
}

#endif