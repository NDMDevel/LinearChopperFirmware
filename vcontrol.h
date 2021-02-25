/* 
 * File:   vcontrol.h
 * Author: Damian
 *
 * Created on December 10, 2019, 7:57 PM
 */

#ifndef VCONTROL_H
#define	VCONTROL_H

#include <stdint.h>
#include <stdbool.h>

#define VDC_MAX             800
#define ADC_SCALE_FACTOR    1.05097
#define FLASH_START_ADDRESS 0x1F00
#define CHOPPER_ENABLED     0x0ABC
#define CHOPPER_DISABLED    0x0CBA

void ADC_VoltageControlHandler_ISR(void);
void TMR2_DutyControlHandler_ISR(void);

void init_chopper();
void start_chopper();
void stop_chopper();
bool is_chopper_active();

void init_relay_watchdog();
void start_relay_watchdog();
void stop_relay_watchdog();
void relay_watchdog_task();
bool is_relay_watchdog_active();

void set_vdc_min(uint16_t vmin);
void set_vdc_max(uint16_t vmax);
void set_vdc_critic(uint16_t vc);
void set_vdc_speed(uint16_t msDelay);
void set_relay_reset_voltage(uint16_t relay_vthres);
void set_reset_duration(uint16_t reset_dur_ms);
void save_to_flash(void);

uint16_t get_vdc_min(void);
uint16_t get_vdc_max(void);
uint16_t get_vdc_critic(void);
uint16_t get_vdc_speed(void);
uint16_t get_relay_reset_voltage(void);
uint16_t get_reset_duration(void);
uint16_t get_vdc(void);

#endif	/* VCONTROL_H */
