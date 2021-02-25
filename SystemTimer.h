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
extern uint8_t system_seconds;
extern uint8_t system_minutes;

//this timer increments system_counter with an interval of 100ms
#define	TIMER_RESET(T)	T = ((uint8_t)((uint8_t)~system_counter)+((uint8_t)1))
#define TIMER_ELAPSE(T,TE) 	((uint8_t)(system_counter + T) >= ((uint8_t)TE))

//these macros must be used for counting seconds:
#define	TIMER_RESET_SEC(T)	T = ((uint8_t)((uint8_t)~system_seconds)+((uint8_t)1))
#define TIMER_ELAPSE_SEC(T,TE) 	((uint8_t)(system_seconds + T) >= ((uint8_t)TE))

//these macros must be used for counting minutes:
#define	TIMER_RESET_MIN(T)	T = ((uint8_t)((uint8_t)~system_minutes)+((uint8_t)1))
#define TIMER_ELAPSE_MIN(T,TE) 	((uint8_t)(system_minutes + T) >= ((uint8_t)TE))

#define t200ms 2
#define t300ms 3
#define t400ms 4
#define t500ms 5

#define t1s 10
#define t2s 20
#define t5s 50
#define t10s 100

void TMR1_SystemTimer_ISR(void);

#endif	/* SYSTEMTIMER_H */

