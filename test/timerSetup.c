#include "FreeRTOSConfig.h"

#include <pico/stdio.h>
#include <pico/time.h>

volatile uint32_t ulTimerTicks = 0;

struct repeating_timer timer;

uint32_t getTimerValue(void)
{
    return ulTimerTicks;
}

bool timer_callback(repeating_timer_t* rt)
{
    ++ulTimerTicks;
    return true;
}

void timerSetup(void)
{
    bool timer_add = add_repeating_timer_ms(10, timer_callback, NULL, &timer);
}