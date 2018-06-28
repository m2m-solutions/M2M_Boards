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
#ifndef __HERTZ_h__
#define __HERTZ_h__

#include "Arduino.h"

class HertzBoard
{
public:
    void begin();

	float getTemperature();

    void setCellularPower(bool state);
	uint8_t getCellularStatus();

    void setLed(uint8_t red, uint8_t green, uint8_t blue);
    void setLedIntensity(uint8_t percentage);
private:
    uint16_t _ledIntensity = 100;
};

#endif