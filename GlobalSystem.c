/*
 * File:   GlobalSystem.c
 * Author: Damian
 *
 * Created on December 3, 2019, 2:42 PM
 */

#include "GlobalSystem.h"
#include "vcontrol.h"
#include "SolidStateRelay.h"
#include "mcc_generated_files/memory.h"

void ApplicationInit(void)
{
    bool invalid_param = false;
    uint16_t chopper_state = FLASH_ReadWord(FLASH_START_ADDRESS);
    uint16_t val = FLASH_ReadWord(FLASH_START_ADDRESS+1);
    if( val <= 800 )
        set_vdc_min(val);
    else
        invalid_param = true;

    val = FLASH_ReadWord(FLASH_START_ADDRESS+2);
    if( val <= 800 )
        set_vdc_max(val);
    else
        invalid_param = true;

    val = FLASH_ReadWord(FLASH_START_ADDRESS+3);
    if( val <= 800 )
        set_vdc_critic(val);
    else
        invalid_param = true;

    val = FLASH_ReadWord(FLASH_START_ADDRESS+4);
    if( val <= 25500 )
        set_vdc_speed(val);
    else
        invalid_param = true;

    val = FLASH_ReadWord(FLASH_START_ADDRESS+5);
    if( val <= 800 )
        set_relay_reset_voltage(val);
    else
        invalid_param = true;

    val = FLASH_ReadWord(FLASH_START_ADDRESS+6);
    if( val <= 25500 )
        set_reset_duration(val);
    else
        invalid_param = true;

    if( invalid_param == false )
    {
        //if all params are ok including the copper_state (enabled/disabled)
        //then the copper is started/stopped accordingly
        if( chopper_state == CHOPPER_ENABLED || chopper_state == CHOPPER_DISABLED )
        {
            if( chopper_state == CHOPPER_ENABLED )
                start_chopper();
        }
        else
            invalid_param = true;
    }
    else
    {
        //if flash not written or written with invalid data,
        //then some default values are taken
        set_vdc_min(0);
        set_vdc_max(0);
        set_vdc_critic(0);
        set_vdc_speed(0);
        set_relay_reset_voltage(0);
        set_reset_duration(0);
    } 
}
