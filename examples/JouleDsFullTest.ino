//---------------------------------------------------------------------------------------------
//
// Full test sketch for Purplepoint Joule DS.
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
#include <VL53L0X.h>
#include <RHReliableDatagram.h>
#include <RH_RF95.h>

JouleDsBoard board;
VL53L0X distanceSensor;
RH_RF95 radioDriver(PIN_RFM_CS, PIN_RFM_IRQ);
RHReliableDatagram radio(radioDriver);

// the setup function runs once when you press reset or power the board
void setup() {  
  Serial.begin(115200);
  board.begin();
  Serial.println("Purplepoint Joule DS test sketch");
  Serial.println("================================");

  Serial.println("Switching external power on");
  board.setSwitchedPower(true);
  delay(100);
  
  Serial.print("Initializing radio");
  if (!radio.init())
  {
    Serial.println(" - FAIL");
    while (true);
  }
  radioDriver.setTxPower(20, false);
  radioDriver.setFrequency(868.0);
  radio.setTimeout(200);
  radio.setRetries(3);
  Serial.println(" - OK");

  Serial.print("Initializing distance sensor");
  board.setDistanceSensorEnable(true);
  delay(100);
  if (!distanceSensor.init())
  {
    Serial.println(" -FAIL");
    while (1);
  }
  Serial.println(" - OK");  
}

// the loop function runs over and over again forever
void loop() 
{
  Serial.println("-----------------------");
  board.setLed(true);
  
  float value = board.getBatteryVoltage();
  Serial.print("Battery voltage: ");
  Serial.println(value);
  
  value = board.getRegulatedVoltage();
  Serial.print("Regulated voltage: ");  
  Serial.println(value);
  
  value = board.getTemperature();  
  Serial.print("Temperature: ");  
  Serial.println(value);

  uint16_t distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(500);
  board.setLed(false);
  delay(1000);
}

uint16_t getDistance()
{
  uint16_t distance = distanceSensor.readRangeSingleMillimeters();
  if (distanceSensor.timeoutOccurred()) 
  { 
    Serial.print(" TIMEOUT"); 
  }
  return distance;
}
