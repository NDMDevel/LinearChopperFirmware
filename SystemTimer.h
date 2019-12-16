/* 
 * File:   SystemTimer.h
 * Author: Damian
 *
 * Created on December 12, 2019, 8:39 PM
 */

#ifndef SYSTEMTIMER_H
#define	SYSTEMTIMER_H

#include <stdint.h>

extern uint8_t system_counter;

#define	TIMER_RESET(T)	T = ((uint8_t)((uint8_t)~system_counter)+((uint8_t)1))
#define TIMER_ELAPSE(T,TE) 	((uint8_t)(system_counter + T) >= ((uint8_t)TE))

#define t200ms 2
#define t300ms 3
#define t400ms 4
#define t500ms 5

void TMR1_SystemTimer_ISR(void);

#endif	/* SYSTEMTIMER_H */

