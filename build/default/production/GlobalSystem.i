# 1 "GlobalSystem.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "GlobalSystem.c" 2







# 1 "./GlobalSystem.h" 1



# 1 "./vcontrol.h" 1
# 11 "./vcontrol.h"
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\stdint.h" 1 3



# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\musl_xc8.h" 1 3
# 4 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\stdint.h" 2 3
# 22 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\bits/alltypes.h" 1 3
# 135 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned long uintptr_t;
# 150 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long intptr_t;
# 166 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\bits/alltypes.h" 3
typedef signed char int8_t;




typedef short int16_t;




typedef __int24 int24_t;




typedef long int32_t;





typedef long long int64_t;
# 196 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long long intmax_t;





typedef unsigned char uint8_t;




typedef unsigned short uint16_t;




typedef __uint24 uint24_t;




typedef unsigned long uint32_t;





typedef unsigned long long uint64_t;
# 237 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned long long uintmax_t;
# 22 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\stdint.h" 2 3


typedef int8_t int_fast8_t;

typedef int64_t int_fast64_t;


typedef int8_t int_least8_t;
typedef int16_t int_least16_t;

typedef int24_t int_least24_t;

typedef int32_t int_least32_t;

typedef int64_t int_least64_t;


typedef uint8_t uint_fast8_t;

typedef uint64_t uint_fast64_t;


typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;

typedef uint24_t uint_least24_t;

typedef uint32_t uint_least32_t;

typedef uint64_t uint_least64_t;
# 155 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\bits/stdint.h" 1 3
typedef int32_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
# 155 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\stdint.h" 2 3
# 11 "./vcontrol.h" 2

# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.05\\pic\\include\\c99\\stdbool.h" 1 3
# 12 "./vcontrol.h" 2








void ADC_VoltageControlHandler_ISR(void);
void TMR2_DutyControlHandler_ISR(void);

void init_chopper();
void start_chopper();
void stop_chopper();
_Bool is_chopper_active();

void init_relay_watchdog();
void start_relay_watchdog();
void stop_relay_watchdog();
void relay_watchdog_task();
_Bool is_relay_watchdog_active();

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
# 4 "./GlobalSystem.h" 2

# 1 "./SolidStateRelay.h" 1
# 22 "./SolidStateRelay.h"
void init_relay_watchdog(void);
void start_relay_watchdog(void);
void stop_relay_watchdog(void);
_Bool is_relay_watchdog_active(void);

void set_relay_reset_voltage(uint16_t relay_vthres);
void set_reset_duration(uint16_t reset_dur_ms);
uint16_t get_relay_reset_voltage(void);
uint16_t get_reset_duration(void);

void relay_watchdog_task(void);
void relay_watchdog_record_activations_task(void);

void reset_activation_counter(void);
uint32_t get_relay_activation_counter(void);
void set_relay_activation_counter(uint32_t act_count);


static void close_relay(void);
static void open_relay(void);
# 5 "./GlobalSystem.h" 2


void ApplicationInit(void);
# 8 "GlobalSystem.c" 2



# 1 "./mcc_generated_files/memory.h" 1
# 99 "./mcc_generated_files/memory.h"
uint16_t FLASH_ReadWord(uint16_t flashAddr);
# 128 "./mcc_generated_files/memory.h"
void FLASH_WriteWord(uint16_t flashAddr, uint16_t *ramBuf, uint16_t word);
# 164 "./mcc_generated_files/memory.h"
int8_t FLASH_WriteBlock(uint16_t writeAddr, uint16_t *flashWordArray);
# 189 "./mcc_generated_files/memory.h"
void FLASH_EraseBlock(uint16_t startAddr);
# 11 "GlobalSystem.c" 2


void ApplicationInit(void)
{
    _Bool invalid_param = 0;
    uint16_t chopper_state = FLASH_ReadWord(0x1F00);
    uint16_t val = FLASH_ReadWord(0x1F00 +1);
    if( val <= 800 )
        set_vdc_min(val);
    else
        invalid_param = 1;

    val = FLASH_ReadWord(0x1F00 +2);
    if( val <= 800 )
        set_vdc_max(val);
    else
        invalid_param = 1;

    val = FLASH_ReadWord(0x1F00 +3);
    if( val <= 800 )
        set_vdc_critic(val);
    else
        invalid_param = 1;

    val = FLASH_ReadWord(0x1F00 +4);
    if( val <= 10000 )
        set_vdc_speed(val);
    else
        invalid_param = 1;

    val = FLASH_ReadWord(0x1F00 +5);
    if( val <= 800 )
        set_relay_reset_voltage(val);
    else
        invalid_param = 1;

    val = FLASH_ReadWord(0x1F00 +6);
    if( val <= 10000 )
        set_reset_duration(val);
    else
        invalid_param = 1;

    if( invalid_param == 0 )
    {


        if( chopper_state == 0x0ABC || chopper_state == 0x0CBA )
        {
            if( chopper_state == 0x0ABC )
                start_chopper();
        }
        else
            invalid_param = 1;
    }
    else
    {


        set_vdc_min(0);
        set_vdc_max(0);
        set_vdc_critic(0);
        set_vdc_speed(0);
        set_relay_reset_voltage(0);
        set_reset_duration(0);
    }
    val = FLASH_ReadWord(0x1F00 +7);
    uint32_t act_counter = val;
    act_counter <<= 16;
    val = FLASH_ReadWord(0x1F00 +8);
    act_counter |= val;
    if( act_counter == 0x3FFF3FFF )
    {
        act_counter = 0;
        set_relay_activation_counter(act_counter);
        save_to_flash();
    }
    else
        set_relay_activation_counter(act_counter);
}
