//---------------------------------------------------------------------------------------------
//
// Library for the ATMEGA328P platform.
//
// Copyright 2016-2020, M2M Solutions AB
// Written by Jonny Bergdahl, 2020-04-05
//
// Licensed under the MIT license, see the LICENSE.txt file.
//
//---------------------------------------------------------------------------------------------
#ifndef __avrcore_h__
#define __avrcore_h__

class Watchdog
{
public:
	Watchdog():
		wdtoValue(-1)
	{}
	int enable(int maxPeriod = 0, bool forSleep = false);
	void disable();
	void reset();
	int sleep(int period = 0);

private:
	uint16_t wdtoValue;
};

#endif