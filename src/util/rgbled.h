//---------------------------------------------------------------------------------------------
//
// Library for the SK6812/WS2812B serial RGB LED types.
//
// Copyright 2018, M2M Solutions AB
// Written by Jonny Bergdahl, 2018-07-06
//
// Licensed under the MIT license, see the LICENSE.txt file.
//
//---------------------------------------------------------------------------------------------
#ifndef __util_rgbled_h__
#define  __util_rgbled_h__

#include <Arduino.h>

void setRgbLed(uint8_t pin, uint8_t red, uint8_t green, uint8_t blue, uint8_t intensity);

#endif