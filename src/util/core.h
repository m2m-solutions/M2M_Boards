//---------------------------------------------------------------------------------------------
//
// Library for the ATSAMD21 (ARM M0) platform.
//
// Copyright 2016-2018, M2M Solutions AB
// Written by Jonny Bergdahl, 2018-06-29
//
// Licensed under the MIT license, see the LICENSE.txt file.
//
//---------------------------------------------------------------------------------------------
#ifndef __core_h__
#define __core_h__

class Watchdog
{
public:
	int enable(int maxPeriod = 0, bool forSleep = false);
	void disable();
	void reset();
	int sleep(int period = 0);

private:
	int initialize();
	bool isInitialized = false;
};
#endif