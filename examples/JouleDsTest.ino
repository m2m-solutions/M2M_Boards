//---------------------------------------------------------------------------------------------
//
// Simplified test sketch for Purplepoint Joule DS.
//
// Copyright 2020, M2M Solutions AB
// Written by Jonny Bergdahl, 2020-04-12
//
// Licensed under the MIT license, see the LICENSE.txt file.
//
// This sketch does not require any external libraries.
//
////////////////////////////////////////////////////////////////////////////////////////////////
#include "JouleDsBoard.h"

JouleDsBoard board;

// the setup function runs once when you press reset or power the board
void setup() {  
  Serial.begin(115200);
  board.begin();
  Serial.println("Purplepoint Joule DS test sketch");
  Serial.println("================================");

  Serial.println("Switching external power on");
  board.setSwitchedPower(true);  
}

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

  delay(500);
  board.setLed(false);
  delay(1000);
}
