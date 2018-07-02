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
#ifndef __HERTZ_h__
#define __HERTZ_h__

#include "Arduino.h"
#include "util/core.h"

class HertzBoard
{
public:
    HertzBoard();
    
    void begin();

	float getTemperature();

    void setCellularPower(bool state);
	uint8_t getCellularStatus();

    void setLed(uint8_t red, uint8_t green, uint8_t blue);
    void setLedIntensity(uint8_t percentage);
    
    Watchdog watchdog;
private:
    uint16_t _ledIntensity = 100;
};

#endif