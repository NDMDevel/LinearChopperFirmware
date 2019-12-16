#ifndef GLOBALPARAMETERS_H
#define	GLOBALPARAMETERS_H

#include <stdint.h>

uint16_t    getDeltaT(void);
void        setDeltaT(uint16_t delta_t);

uint16_t    getVoltageHigh(void);
void        setVoltageHigh(uint16_t voltage_h);

uint16_t    getVoltageLow(void);
void        setVoltageLow(uint16_t voltage_l);

#endif	/* GLOBALPARAMETERS_H */

