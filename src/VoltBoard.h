//---------------------------------------------------------------------------------------------
//
// Library for the Purplepoint Hertz board.
//
// Copyright 2016-2018, M2M Solutions AB
// Written by Jonny Bergdahl, 2018-04-19
//
// Licensed under the MIT license, see the LICENSE.txt file.
//
//---------------------------------------------------------------------------------------------
#ifndef __VOLTBOARD_h__
#define __VOLTBOARD_h__

#ifdef ARDUINO_PP_VOLT

#include "Arduino.h"
#include "util/core.h"

class VoltBoard
{
public:
    VoltBoard();
    
    void begin();

    static const char* getSerialNumber();
    static const char* getEncryptionKey();
    static const char* getHashKey();

	float getTemperature();

    void setSwitchedPower(bool state);

    void setLed(uint8_t red, uint8_t green, uint8_t blue);
    void setLedIntensity(uint8_t percentage);
    
    Watchdog watchdog;
private:
    uint16_t _ledIntensity = 100;
};

#endif
#endif