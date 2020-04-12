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

class JouleDsBoard
{
public:
    JouleDsBoard();
    
    void begin();

    static void getSerialNumber(char* buffer);
    static void getMqttPassword(char* buffer);
    static void getEncryptionKey(char* buffer);
    static void getHashKey(char* buffer);

	float getTemperature();
    float getTemperatureInKelvin();
    float getTemperatureInFarenheit();

	float getBatteryVoltage();
    float getRegulatedVoltage();

    void setSwitchedPower(bool state);
    void setDistanceSensorEnable(bool state);

    void setLed(bool state);

    Watchdog watchdog;
private:
    static void CopyEepromString(uint16_t address, char* buffer);
};

#endif
#endif