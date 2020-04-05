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
#ifndef __JOULEDSBOARD_h__
#define __JOULEDSBOARD_h__

#ifdef ARDUINO_PP_JOULE_DS

#include "Arduino.h"
#include "util/avrcore.h"

class HertzDsBoard
{
public:
    HertzBoard();
    
    void begin();

    static const char* getSerialNumber(char* buffer);
    static const char* getMqttPassword(char* buffer);
    static const char* getEncryptionKey(char* buffer);
    static const char* getHashKey(cahr* buffer);

	float getTemperature();
    float getTemperatureInKelvin();
    float getTemperatureInFarenheit();

	float getBatteryVoltage();
    
    void setSwitchedPower(bool state);

    Watchdog watchdog;
private:
    void CopyEepromString(uint16_t address);
};

#endif
#endif