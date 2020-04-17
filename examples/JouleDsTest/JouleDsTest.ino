//---------------------------------------------------------------------------------------------
//
// Test sketch for Purplepoint Joule DS.
//
// Copyright 2020, M2M Solutions AB
// Written by Jonny Bergdahl, 2020-04-12
//
// Licensed under the MIT license, see the LICENSE.txt file.
//
// This sketch requires the following libraries:
//  - http://www.airspayce.com/mikem/arduino/RadioHead/
//  - https://github.com/pololu/vl53l0x-arduino
//
////////////////////////////////////////////////////////////////////////////////////////////////
#include "JouleDsBoard.h"
#include <Wire.h>

JouleDsBoard board;

// the setup function runs once when you press reset or power the board
void setup() {  
  Serial.begin(115200);
  board.begin();
  Serial.println(F("Purplepoint Joule DS test sketch"));
  Serial.println(F("================================"));

  Serial.println(F("Switching external power on"));
  board.setSwitchedPowerEnabled(true);
  delay(100);
  
  Serial.print(F("Initializing radio"));
  if (!board.setRadioEnabled(true))
  {
    Serial.println(F(" - FAIL"));
    while (true);
  }
  board.setRadioTxPower(20);
  board.setRadioTimeout(200);
  board.setRadioRetries(3);
  Serial.println(F(" - OK"));

  Serial.print(F("Initializing distance sensor"));
  board.setDistanceSensorEnabled(true);
  delay(100);
  if (!board.distanceSensor.init())
  {
    Serial.println(F(" -FAIL"));
    while (1);
  }
  Serial.println(F(" - OK"));  
}

// the loop function runs over and over again forever
void loop() 
{
  Serial.println(F("-----------------------"));
  board.setLed(true);
  
  float value = board.getBatteryVoltage();
  Serial.print(F("Battery voltage: "));
  Serial.println(value);
  
  value = board.getRegulatedVoltage();
  Serial.print(F("Regulated voltage: "));  
  Serial.println(value);
  
  value = board.getTemperature();  
  Serial.print(F("Temperature: "));  
  Serial.println(value);

  uint16_t distance = getDistance();
  Serial.print(F("Distance: "));
  Serial.println(distance);
  delay(500);
  board.setLed(false);
  delay(1000);
}

uint16_t getDistance()
{
  uint16_t distance = board.distanceSensor.readRangeSingleMillimeters();
  if (board.distanceSensor.timeoutOccurred()) 
  { 
    Serial.print(F(" TIMEOUT")); 
  }
  return distance;
}
