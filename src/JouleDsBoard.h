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
#include <VL53L0X.h>
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <M2M_Logger.h>

#define JOULE_DS_DEBUG

class JouleDsBoard
{

public:
    JouleDsBoard();
    
    void begin();

    void setLogger(Logger* logger);

    static void getSerialNumber(char* buffer);
    static void getMqttPassword(char* buffer);
    static void getEncryptionKey(char* buffer);
    static void getHashKey(char* buffer);

	float getTemperature();
    float getTemperatureInKelvin();
    float getTemperatureInFarenheit();

	float getBatteryVoltage();
    float getRegulatedVoltage();

    void setSwitchedPowerEnabled(bool state);
    bool getSwitchedPowerEnabled();

    bool setDistanceSensorEnabled(bool state);
    float getMeasuredDistance();

    bool setRadioEnabled(bool state, bool longRange = false);
    void setRadioTxPower(uint8_t level);
    void setRadioTimeout(uint16_t timeout);
    void setRadioRetries(uint8_t count);
    void setRadioModemRegisters(const RH_RF95::ModemConfig* config);
    void setRadioModemConfig(RH_RF95::ModemConfigChoice index);
    void setRadioSpreadingFactor(uint8_t factor);
    uint16_t getRadioLastSNR();

    void setLed(bool state);

    Watchdog watchdog;
    VL53L0X distanceSensor;
    RH_RF95 radioDriver;
    RHReliableDatagram radio;    

private:
    bool _distanceSensorEnabled;
    bool _radioEnabled;
    bool _radioLongRange;
    Logger* _logger;
    static void CopyEepromString(uint16_t address, char* buffer);
    void sortArray(uint16_t values[], uint8_t count);
};

#endif
#endif