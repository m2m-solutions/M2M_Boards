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
#define VOLTAGE_CALIBRATION		HASH_KEY_ADDR + 24
#define INVALID_TEMPERATURE		-1000.0
#define DS_MEASURE_COUNT		1

#ifdef JOULE_DS_DEBUG
#define DS_ERROR(...) if (_logger != nullptr) _logger->error(__VA_ARGS__)
#define DS_INFO(...) if (_logger != nullptr) _logger->info(__VA_ARGS__)
#define DS_DEBUG(...) if (_logger != nullptr) _logger->debug(__VA_ARGS__)
#define DS_TRACE(...) if (_logger != nullptr) _logger->trace(__VA_ARGS__)
#define DS_TRACE_START(...) if (_logger != nullptr) _logger->traceStart(__VA_ARGS__)
#define DS_TRACE_PART(...) if (_logger != nullptr) _logger->tracePart(__VA_ARGS__)
#define DS_TRACE_END(...) if (_logger != nullptr) _logger->traceEnd(__VA_ARGS__)
#else
#define DS_ERROR(...)
#define DS_INFO(...)
#define DS_DEBUG(...)
#define DS_TRACE(...)
#define DS_TRACE_START(...)
#define DS_TRACE_PART(...)
#define DS_TRACE_END(...)
#endif

JouleDsBoard::JouleDsBoard()
    : watchdog(),
	radioDriver { PIN_RFM_CS, PIN_RFM_IRQ },
	radio { radioDriver },
    _distanceSensorEnabled { false },
    _radioEnabled { false },
	_radioLongRange { false },
	_logger { NULL }
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

//---------------------------------------------------------------------------------------------
// Board functions

void JouleDsBoard::setLogger(Logger* logger)
{
	_logger = logger;
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
	const float vcc = 3.3;	// Joule DS has a linear voltage regulator @3.3V +-1%
	digitalWrite(PIN_PWR_AD_EN, HIGH);	
	delay(20);	// We have 100nF cap on the ADC input, just let let it charge up first
	uint16_t value = analogRead(PIN_PWR_AD);
	digitalWrite(PIN_PWR_AD_EN, LOW);
	float percentage = 2 * value / 1024.0;
	return (percentage * vcc);
}

float JouleDsBoard::getRegulatedVoltage()
{
	return readVcc();
}

void JouleDsBoard::setSwitchedPowerEnabled(bool state)
{
	DS_TRACE(F("Switched power: %i"), state);
	digitalWrite(PIN_VSW_EN, !state);
	if (!state)
	{
		_radioEnabled = false;
	}
}

bool JouleDsBoard::getSwitchedPowerEnabled()
{
	return digitalRead(PIN_VSW_EN);
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

//---------------------------------------------------------------------------------------------
// Distance sensor

bool JouleDsBoard::setDistanceSensorEnabled(bool state)
{
	DS_TRACE(F("Distance sensor enabled: %i"), state);
	if (state)
	{
		digitalWrite(PIN_DSXSHUT, true);
  		if (!distanceSensor.init())
  		{
		    return false;
  		}
		distanceSensor.setTimeout(500);
		distanceSensor.setMeasurementTimingBudget(66000);
	}
	else
	{
		digitalWrite(PIN_DSXSHUT, false);
	}
	return true;
}

float JouleDsBoard::getMeasuredDistance()
{
	uint16_t distances[DS_MEASURE_COUNT];
	for (int i = 0; i < DS_MEASURE_COUNT; i++)
  	{
    	distances[i] = distanceSensor.readRangeSingleMillimeters();
		DS_TRACE("Measured distance: %i", distances[i]);
  	}
  	sortArray(distances, DS_MEASURE_COUNT);
  	uint16_t measuredDistance = distances[DS_MEASURE_COUNT / 2];
	return (float)measuredDistance / 1000.0;  	
}

//---------------------------------------------------------------------------------------------
// Radio 

bool JouleDsBoard::setRadioEnabled(bool state, bool longRange)
{
	DS_TRACE(F("Radio enabled: %i"), state);
	_radioEnabled = false;
	if (state)
	{
		if (!radio.init())
		{
			DS_ERROR("radio.init failed");
			return false;
		}
  		radioDriver.setFrequency(868.0);
		radioDriver.setTxPower(20, true);
		radio.setRetries(3);
		if (longRange)
		{
			radio.setTimeout(1000);
			RH_RF95::ModemConfig modem_config = {
    			0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
    			0xC4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
    			0x08  // Reg 0x26: LowDataRate=On, Agc=Off.  0x0C is LowDataRate=ON, ACG=ON
  			};
  			radioDriver.setModemRegisters(&modem_config);
  			if (!radioDriver.setModemConfig(RH_RF95::Bw125Cr48Sf4096))
  			{
				DS_ERROR(F("setModemConfig failed"));
				return false;
  			}			
		}
		else
		{
			radio.setTimeout(200);			
		}
		_radioEnabled = true;
	}
	delay(50);
	return true;
}

void JouleDsBoard::setRadioTxPower(uint8_t level)
{
	if (_radioEnabled)
	{
		radioDriver.setTxPower(level, true);
	}
}

void JouleDsBoard::setRadioTimeout(uint16_t timeout)
{
	if (_radioEnabled)
	{
		radio.setTimeout(timeout);
	}
}

void JouleDsBoard::setRadioRetries(uint8_t count)
{
	if (_radioEnabled)
	{
		radio.setRetries(count);
	}
}

void JouleDsBoard::setRadioModemRegisters(const RH_RF95::ModemConfig* config)
{
	if (_radioEnabled)
	{		
		radioDriver.setModemRegisters(config);
	}
}

void JouleDsBoard::setRadioModemConfig(RH_RF95::ModemConfigChoice index)
{
	if (_radioEnabled)
	{
		radioDriver.setModemConfig(index);
	}
}

void JouleDsBoard::setRadioSpreadingFactor(uint8_t factor)
{
	if (_radioEnabled)
	{
		radioDriver.setSpreadingFactor(factor);
	}
}

uint16_t JouleDsBoard:: getRadioLastSNR()
{
	return radioDriver.lastSNR();
}

void JouleDsBoard::sortArray(uint16_t values[], uint8_t count)
{
	uint16_t min_idx;  
  
    // One by one move boundary of unsorted subarray  
    for (uint16_t i = 0; i < count - 1; i++)  
    {  
        // Find the minimum element in unsorted array  
        min_idx = i;  
        for (uint16_t j = i + 1; j < count; j++)
		{
        	if (values[j] < values[min_idx])
			{
            	min_idx = j;
			}
		}
        // Swap the found minimum element with the first element
		uint16_t temp = values[min_idx];
		values[i] = values[min_idx];
		values[min_idx] = temp;
    }
}

#endif