/* 
 * File:   SolidStateRelay.h
 * Author: Damian
 *
 * Created on February 23, 2021, 8:22 AM
 */

#ifndef SOLIDSTATERELAY_H
#define	SOLIDSTATERELAY_H

#include <stdint.h>
#include <stdbool.h>

/*
 * OPEN: NC disconected from C
 * CLOSE: NC connected to C
 * 
 * open_relay() will send 5V to the relay (activating it)
 * close_relay() sends GND to the relay (deactivating it, release state)
 */

void init_relay_watchdog(void);
void start_relay_watchdog(void);
void stop_relay_watchdog(void);
bool is_relay_watchdog_active(void);

void set_relay_reset_voltage(uint16_t relay_vthres);
void set_reset_duration(uint16_t reset_dur_ms);
uint16_t get_relay_reset_voltage(void);
uint16_t get_reset_duration(void);

void relay_watchdog_task(void);
void relay_watchdog_record_activations_task(void);

void reset_activation_counter(void);
uint32_t get_relay_activation_counter(void);
void set_relay_activation_counter(uint32_t act_count);

//private methods
static void close_relay(void);
static void open_relay(void);


#endif	/* SOLIDSTATERELAY_H */

