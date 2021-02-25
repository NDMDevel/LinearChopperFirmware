#include "SolidStateRelay.h"
#include "vcontrol.h"
#include "SystemTimer.h"
#include "mcc_generated_files/pin_manager.h"

//RELAY WATCHDOG
enum RelayWatchdog
{
    SHUTDOWN = 0,
    INIT,
    WAIT_VOLTAGE_RISE,
    WAIT_VOLTAGE_FALL,
    WAIT_RESET_DURATION
};
static enum RelayWatchdog st = SHUTDOWN;
static uint16_t relay_reset_voltage;
static uint16_t reset_duration;
static uint16_t prev_vdc;
static uint8_t local_timer;
static uint16_t reset_duration_ms;
static uint32_t activation_counter = 0; 

void init_relay_watchdog()
{
    //initialization goes here...
}

void start_relay_watchdog()
{
    if( st == SHUTDOWN )
        st = INIT;
}

void stop_relay_watchdog()
{
    st = SHUTDOWN;
    close_relay();
}

bool is_relay_watchdog_active()
{
    return st != SHUTDOWN;
}

void set_relay_reset_voltage(uint16_t relay_vthres)
{
    if( relay_vthres == 0xFFFF )
        return;
    if( relay_vthres > 800 )
        relay_reset_voltage = 800;
    else
        relay_reset_voltage = relay_vthres;
}

void set_reset_duration(uint16_t reset_dur_ms)
{
    if( reset_dur_ms == 0xFFFF )
        return;
    if( reset_dur_ms > 25500 )
        reset_duration = 25500;
    else
        reset_duration = reset_dur_ms;
    
    //reset_duration_ms can be directly used with TIMER_ELAPSE, since
    //the system timer increments with intervals of 100ms
    reset_duration_ms = reset_duration/100;
}

uint16_t get_relay_reset_voltage(void)
{
    return relay_reset_voltage;
}

uint16_t get_reset_duration(void)
{
    return reset_duration;
}

void reset_activation_counter(void)
{
    activation_counter = 0;
}

uint32_t get_activation_counter(void)
{
    return activation_counter;
}


static void open_relay(void)
{
    RELAY_GPIO_SetHigh();
}

static void close_relay(void)
{
    RELAY_GPIO_SetLow();
}

void relay_watchdog_task()
{
    if( st == SHUTDOWN )
        return;
    if( st == INIT )
    {
        //close the relay and start the state machine
        close_relay();
        st = WAIT_VOLTAGE_RISE;
        return;
    }
    if( st == WAIT_VOLTAGE_RISE )
    {
        //if voltage not rising, retuns
        if( get_vdc() <= relay_reset_voltage )
            return;
        
        //voltage is rising, now wait for it to fall below threshold
        //to open the relay
        st = WAIT_VOLTAGE_FALL;
        return;
    }
    if( st == WAIT_VOLTAGE_FALL )
    {
        //voltage fall below threshold ??
        if( get_vdc() >= relay_reset_voltage )
            return; //no, returns
        
        //the voltage fall below threshold, so the relay must open:
        open_relay();
        activation_counter++;
        TIMER_RESET(local_timer);
        st = WAIT_RESET_DURATION;
        return;
    }
    if( st == WAIT_RESET_DURATION )
    {
        //keep the relay opened for the specified among of time:
        if( !TIMER_ELAPSE(local_timer,reset_duration_ms) )
            return;
        
        //now the relay must be close again:
        close_relay();
        //now goes to wait again for the woltage to rise and repeat the process:
        st = WAIT_VOLTAGE_RISE;
        return;
    }
}
