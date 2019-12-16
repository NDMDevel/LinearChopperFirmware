/*
 * File:   GlobalParameters.c
 * Author: Damian
 *
 * Created on December 3, 2019, 2:42 PM
 */

#include "GlobalParameters.h"

static uint16_t DeltaT;
static uint16_t VoltageHigh;
static uint16_t VoltageLow;

uint16_t getDeltaT(void)
{
    //apply conversions
    return DeltaT;
}

void setDeltaT(uint16_t delta_t)
{
    //apply inv conversions
    DeltaT = delta_t;
}

uint16_t getVoltageHigh(void)
{
    //apply conversions
    return VoltageHigh;
}

void setVoltageHigh(uint16_t voltage_h)
{
    //apply inv conversions
    VoltageHigh = voltage_h;
}

uint16_t getVoltageLow(void)
{
    //apply conversions
    return VoltageLow;
}

void setVoltageLow(uint16_t voltage_l)
{
    //apply inv conversions
    VoltageLow = voltage_l;
}