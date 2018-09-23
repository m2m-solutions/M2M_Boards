//---------------------------------------------------------------------------------------------
//
// PurplePoint Volt/Hertz Temperature Client
//
// Copyright 2018, M2M Solutions AB
// Jonny Bergdahl, 2018-09-23
//
//---------------------------------------------------------------------------------------------
// TODO: 
//---------------------------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////////////////////////
//
// Project configuration defines
//
// Standard defines
#define FIRMWARE_VERSION		"1.0"							// Firmware version
#define DATA_SEND_INTERVAL 10000              // Milliseconds
//
// Mira defines
#define MIRA_ANTENNA_TYPE   MiraAntenna::internal   // internal or external 
#define MIRA_DEBUG_LOGLEVEL   LogLevel::Debug
//
// MQTT defines
#define MQTT_TOPIC_PREFIX		"pp"							// Purplepoint
#define MAX_RETRY_MQTT			30								// Max number of seconds connecting to MQTT server
#define MQTT_SERVER				"services.m2msolutions.se"		// MQTT server name
#define MQTT_PORT				1883							// MQTT server port
//
// Debug defines
#define DEBUG_SERIAL      SerialUSB
#define DEBUG_LOGLEVEL    LogLevel::Trace

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
#include <Arduino.h>
#ifdef ARDUINO_PP_VOLT
#include <VoltBoard.h>
#endif 
#ifdef ARDUINO_PP_HERTZ
#include <HertzBoard.h>
#endif 
#include <M2M_Logger.h>
#include <M2M_MiraOne.h>
#include <M2M_MiraOneMessage.h>
#include <M2M_MiraOnePayload.h>
#include <M2M_TLV.h>
#include <M2M_TLV_ID.h>
#include <Wire.h>

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Internal defines
//
#define WATCHDOG_TIMEOUT	8000	// 8 seconds

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Platform specific defines
//
#if !defined(ARDUINO_PP_VOLT) && !defined(ARDUINO_PP_HERTZ)
#error This code is written for the Purplepoint Volt or Hertz boards
#endif

#ifdef ARDUINO_PP_VOLT
VoltBoard board;
#endif
#ifdef ARDUINO_PP_HERTZ
HertzBoard board;
#endif
VersionInfo miraVersion;
Logger Log;
Logger miraLogger;
MiraOne mira(SerialRadio, PIN_MRESET);
uint32_t timer = 0;
IEEE_EUI64 miraAddress;
const char* firmwareVersion = FIRMWARE_VERSION;
uint32_t lastAck = 0;

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Setup
//
void setup()
{  
  while (!SerialUSB);
  
  setupBoard();
  setupLogger();
  board.setLed(16, 16, 0);   // Yellow
	Log.info("PurplePoint Volt temperature node");
	Log.info("Serial number: %s, Firmware version: %s", board.getSerialNumber(), firmwareVersion);

  setupMiraOne();

	timer = millis() + DATA_SEND_INTERVAL;
  board.setLed(0, 16, 0);   // Green
  Log.info("Entering loop");
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Loop
//
void loop()
{
	board.watchdog.reset();
	if (mira.available())
	{
		processData();
	}
	if (millis() > timer)
	{
    Log.info("Timer elapsed");
		sendData();
		timer = millis() + DATA_SEND_INTERVAL;
	}
	delay(10);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Setup functions
//
void setupBoard()
{
  board.begin();
#ifdef ARDUINO_PP_VOLT  
  board.setSwitchedPower(true);
#endif  
  delay(500);
  float temperature = board.getTemperatureInKelvin();
  board.watchdog.enable(8000);
}

void setupLogger()
{
  SerialUSB.begin(115200);
  Log.begin(&DEBUG_SERIAL, DEBUG_LOGLEVEL);
  Log.setIncludeLogLevel(true);
  Log.setIncludeTimestamp(true);
  miraLogger.begin(&DEBUG_SERIAL, MIRA_DEBUG_LOGLEVEL);
  miraLogger.setIncludeLogLevel(true);
  miraLogger.setIncludeTimestamp(true);
}

void setupMiraOne()
{
  
  Log.info("Setup mira");
  SerialRadio.begin(115200);
  mira.setLogger(&miraLogger);
  mira.setWatchdogCallback(watchdogCallback);
  mira.begin(false, board.getSerialNumber());
  
  Log.debug("Getting address");
  if (!mira.getEUI64Info(&miraAddress))
  {
    fail("Failed getting MiraOne address");
  }
  Log.traceStart("MiraOne address: ");
  Log.tracePartHexDump(&miraAddress, 8);
  Log.traceEnd("");
  
  Log.debug("Getting version");
  if (!mira.getVersion(miraVersion))
  {
    fail("Failed getting MiraOne version");
  }   
  Log.trace("MiraOne firmware version: %i.%i", miraVersion.major, miraVersion.minor);
}
////////////////////////////////////////////////////////////////////////////////////////////////
//
// Send / Receive functions
//

void sendData()
{  
  const uint8_t bufferSize = 128;
  uint8_t buffer[bufferSize];
  MiraOnePayloadv1* payload = (MiraOnePayloadv1*)&buffer;
  Log.info("Sending node data");

  float temperature = board.getTemperatureInKelvin();
  if (temperature == LM75A_INVALID_TEMPERATURE)
  {
    fail("Failed to read temperature");
  }
  Log.debug("Temperature: %i.%i", (uint16_t)temperature, (uint16_t)(temperature * 100) % 100);
  M2M_TLV::Input inputArray[] = {
    M2M_TLV::objectInstance(0),
    M2M_TLV::  string((uint16_t)ResourceId::SerialNumber, 12, board.getSerialNumber()),
    M2M_TLV::  string((uint16_t)ResourceId::DeviceType, 1, "n"),
    M2M_TLV::  multipleResource((uint16_t)ResourceId::Temperature),
    M2M_TLV::    float32RI(5700, temperature),
    M2M_TLV::  multipleResource(11),
    M2M_TLV::    integerRI(0, 0)
  };
  M2M_TLV tlv = M2M_TLV(payload->data, bufferSize, inputArray, sizeof(inputArray)/sizeof(M2M_TLV::Input));

    // Setup data buffer
  memcpy(payload->miraAddress.data, miraAddress.data, 8);
  memcpy(payload->serialNumber, board.getSerialNumber(), 12);
  memcpy(payload->data, tlv.getBuffer(), tlv.getBufferSize());
  payload->dataLength = (uint8_t)tlv.getBufferSize();

  Log.traceStart("Payload: ");
  Log.tracePartHexDump(payload, payload->getLength());
  Log.traceEnd("");

  MiraOneMessage* message = MiraOneMessage::getDataSendMessageForRoot((uint8_t*)payload, payload->getLength());
  //message->dumpToLog(&Log);
  if (!mira.send(message))
  {
    Log.error("Unable to to send message");
  }

  // TODO: Wait for send ACK
  delete message;  
}

void processData()
{
	Log.info("Receiving message");
  MiraOneMessage* message = mira.getNextMessage();
  message->dumpToLog(&Log);
  
	uint8_t messageClass = message->getMessageClass();
	uint8_t messageType = message->getMessageType();

	if (messageClass == MIRA_MESSAGE_CLASS_DATAMESSAGE &&
		messageType == MIRA_MESSAGE_TYPE_DATA_RECEIVED)
	{
    Log.debug("Received ACK message");
    lastAck = millis();
	}
	delete message;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Watchdog functions
//

void watchdogCallback()
{
  board.watchdog.reset();
}

void fail(const char* errorMessage)
{
  board.setLed(16, 0, 0);   // Red
  Log.error(errorMessage);
  board.watchdog.reset();   // Reset watchdog, then wait for it to trigger
  while(true);
}
