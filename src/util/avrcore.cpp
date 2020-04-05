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
#include "avrcore.h"
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

ISR(WDT_vect) {
    // Jst defined to prevent reset
}

uint16_t Watchdog::enable(uint16_t maxPeriod, bool forSleep)
{
    uint16_t actualPeriod;
    if((maxPeriod >= 8000) || (maxPeriod == 0)) {
        wdtoValue    = WDTO_8S;
        actualPeriod = 8000;
    } else if(maxPeriod >= 4000) {
        wdtoValue    = WDTO_4S;
        actualPeriod = 4000;
    } else if(maxPeriod >= 2000) {
        wdtoValue    = WDTO_2S;
        actualPeriod = 2000;
    } else if(maxPeriod >= 1000) {
        wdtoValue    = WDTO_1S;
        actualPeriod = 1000;
    } else if(maxPeriod >= 500) {
        wdtoValue    = WDTO_500MS;
        actualPeriod = 500;
    } else if(maxPeriod >= 250) {
        wdtoValue    = WDTO_250MS;
        actualPeriod = 250;
    } else if(maxPeriod >= 120) {
        wdtoValue    = WDTO_120MS;
        actualPeriod = 120;
    } else if(maxPeriod >= 60) {
        wdtoValue    = WDTO_60MS;
        actualPeriod = 60;
    } else if(maxPeriod >= 30) {
        wdtoValue    = WDTO_30MS;
        actualPeriod = 30;
    } else {
        wdtoValue    = WDTO_15MS;
        actualPeriod = 15;
    }
    if (forSleep)
    {
        // Build watchdog prescaler register value before timing critical code.
        uint8_t wdps = ((sleepWDTO & 0x08 ? 1 : 0) << WDP3) |
                    ((sleepWDTO & 0x04 ? 1 : 0) << WDP2) |
                    ((sleepWDTO & 0x02 ? 1 : 0) << WDP1) |
                    ((sleepWDTO & 0x01 ? 1 : 0) << WDP0);

        // The next section is timing critical so interrupts are disabled.
        cli();
        // First clear any previous watchdog reset.
        MCUSR &= ~(1<<WDRF);
        // Now change the watchdog prescaler and interrupt enable bit so the
        // watchdog reset only triggers the interrupt (and wakes from deep sleep)
        // and not a full device reset.  This is a timing critical section of
        // code that must happen in 4 cycles.
        WDTCSR |= (1<<WDCE) | (1<<WDE);  // Set WDCE and WDE to enable changes.
        WDTCSR = wdps;                   // Set the prescaler bit values.
        WDTCSR |= (1<<WDIE);             // Enable only watchdog interrupts.
        // Critical section finished, re-enable interrupts.
        sei(); 

        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();

        // Chip is now asleep!

        // Once awakened by the watchdog execution resumes here.
        // Start by disabling sleep.
        sleep_disable();
    }
    if (wdtoValue != -1)
    {
        wdt_enable(wdtoValue);
    }
    return actualPeriod;			
}

void Watchdog::disable()
{
    wdt_disable();
    wdtoValue = -1;
}

void Watchdog::reset()
{
    wdt_reset();
}

int Watchdog::sleep(int period)
{
    return enable(period, true);
}
