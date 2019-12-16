#include "SystemTimer.h"

uint8_t system_counter;

void TMR1_SystemTimer_ISR(void)
{
    system_counter++;
}

