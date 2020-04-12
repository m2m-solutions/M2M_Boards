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

#ifdef ARDUINO_ARCH_SAMD

#include <sam.h>

class Watchdog
{
public:
	int enable(int maxPeriod = 0, bool forSleep = false);
	void disable();
	void reset();
	int sleep(int period = 0);

private:
	void initialize();
	bool isInitialized = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Copied from Atmel ASF header files
//
enum system_sleepmode {
	/** IDLE 0 sleep mode. */
	SYSTEM_SLEEPMODE_IDLE_0,
	/** IDLE 1 sleep mode. */
	SYSTEM_SLEEPMODE_IDLE_1,
	/** IDLE 2 sleep mode. */
	SYSTEM_SLEEPMODE_IDLE_2,
	/** Standby sleep mode. */
	SYSTEM_SLEEPMODE_STANDBY,
};

enum status_code {
	STATUS_OK                         = 0x00,
	STATUS_ERR_INVALID_ARG            = 0x07,
};
typedef enum status_code status_code_genare_t;

static inline enum status_code system_set_sleepmode(
	const enum system_sleepmode sleep_mode)
{
#if (SAMD20 || SAMD21)
	/* Errata: Make sure that the Flash does not power all the way down
	 * when in sleep mode. */
	NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;
#endif

	switch (sleep_mode) {
		case SYSTEM_SLEEPMODE_IDLE_0:
		case SYSTEM_SLEEPMODE_IDLE_1:
		case SYSTEM_SLEEPMODE_IDLE_2:
			SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
			PM->SLEEP.reg = sleep_mode;
			break;

		case SYSTEM_SLEEPMODE_STANDBY:
			SCB->SCR |=  SCB_SCR_SLEEPDEEP_Msk;
			break;

		default:
			return STATUS_ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

static inline void system_sleep(void)
{
	__DSB();
	__WFI();
}

#endif
#endif