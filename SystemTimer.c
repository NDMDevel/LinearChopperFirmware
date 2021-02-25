#include "SystemTimer.h"

uint8_t system_counter = 0;
static uint8_t _100ms_counter = 0;
static uint8_t _1000ms_counter = 0;
uint8_t system_seconds = 0;
uint8_t system_minutes = 0;

void TMR1_SystemTimer_ISR(void)
{
    system_counter++;
    _100ms_counter++;
    if( _100ms_counter == 10 )
    {
        _100ms_counter = 0;
        system_seconds++;
        
        _1000ms_counter++;
        if( _1000ms_counter == 60 )
        {
            _1000ms_counter = 0;
            system_minutes++;
        }
    }
}

