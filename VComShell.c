/*
 * File:   VComShell.c
 * Author: Damian
 *
 * Created on December 2, 2019, 11:04 PM
 */


#include "VComShell.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <xc.h>
#include "GlobalParameters.h"

static uint8_t rx_buffer[RX_BUFFER_LENGTH];
static uint8_t rx_idx;

static bool command_pending;
static bool command_error;

static const char invalid_com[]     = "Error: invalid com.\r\n";
static const char invalid_arg[]     = "Error: invalid arg.\r\n";

static const char *tx_string = NULL;
static uint8_t tx_idx;
static uint8_t tx_len;
static char num_buffer[5];

static enum
{
    VCS_SHUTDOWN,
    VCS_INIT,
    VCS_SEND_PROMPT,
    VCS_READ_COM,
    VCS_PARSE_COM,
    VCS_PARSE_ARG,
    VCS_SEND_INFO,
    VCS_SEND_INFO_1,
    VCS_SEND_INFO_2,
    VCS_SEND_INFO_3,
    VCS_SEND_INFO_4,
    VCS_SEND_INFO_5,
    VCS_SEND_INFO_6,
    VCS_SEND_INFO_7,
    VCS_SEND_INFO_8,
    VCS_SEND_INFO_9,
    VCS_SEND_ERROR,
    VCS_SEND_OK,
    VCS_SEND_HELP,
    VCS_SEND_HELP_1,
    VCS_SEND_HELP_2,
    VCS_SEND_HELP_3,
    VCS_SEND_HELP_4,
    VCS_SEND_HELP_5,
    VCS_SEND_HELP_6,
    VCS_SEND_HELP_7,
    VCS_SEND_HELP_8,
    VCS_SEND_HELP_9,
    VCS_SEND_HELP_10,
    VCS_SEND_HELP_11
    
}vcs_st = VCS_SHUTDOWN;

static enum
{
    VCS_SUB_SHUTDOWN,
    VCS_SUB_SEND_CHAR,
    VCS_SUB_WAIT_TX_TO_END
            
}vcs_sub_st;

static bool send_task_busy(void);
static void send_string(const char *str,uint8_t len);
static void vcs_send_task(void);
static uint8_t num2string(char *str,uint8_t len,uint16_t num);

//Public methods:
void uart_vcs_isr(void)
{
    uint8_t rx = RC1REG;
    if( command_pending == true )
    {
        PIE1bits.RCIE = 0;
        return;
    }
    if( rx == END_OF_COMMAND_CHAR )
    {
        command_pending = true;
        return;
    }
    if( rx_idx < RX_BUFFER_LENGTH )
        rx_buffer[ rx_idx++ ] = rx;
    else
        command_error = true;
}

void vcs_start(void)
{
    if( vcs_st == VCS_SHUTDOWN )
    {
        vcs_st = VCS_INIT;
        
    }
}

void vcs_stop(void)
{
    PIE1bits.RCIE = 0;
    vcs_st = VCS_SHUTDOWN;
}

void vcs_task(void)
{
    if( vcs_st == VCS_SHUTDOWN )
        return;
    vcs_send_task();
    if( vcs_st == VCS_INIT )
    {
        //software init
        rx_idx = 0;
        command_pending = false;
        command_error   = false;

        //hardware init
        RC1STAbits.FERR = 0;
        RC1STAbits.OERR = 0;
        PIR1bits.RCIF = 0;
        PIE1bits.RCIE = 1;

        vcs_st = VCS_READ_COM;
        return;
    }
    if( vcs_st == VCS_SEND_PROMPT )
    {
        if( send_task_busy() == true )
            return;
        //send HIGH
        send_string(">>  ",4);
        vcs_st = VCS_READ_COM;
        return;
    }
    if( vcs_st == VCS_READ_COM )
    {
        if( command_pending == true )
        {
            if( command_error == true )
            {
                vcs_st = VCS_SEND_ERROR;
                return;
            }
            vcs_st = VCS_PARSE_COM;
        }
        return;
    }
    if( vcs_st == VCS_PARSE_COM )
    {
        switch( rx_buffer[0] )
        {
            case SET_LOW_VOLTAGE:
            case SET_HIGH_VOLTAGE:
            case SET_DELTA_T:
                vcs_st = VCS_PARSE_ARG;
                break;
            case GET_INFO:
                vcs_st = VCS_SEND_INFO;
                break;
            default:
                vcs_st = VCS_SEND_ERROR;
                break;
        }
        return;
    }
    if( vcs_st == VCS_PARSE_ARG )
    {
        uint16_t arg = 0;
        uint8_t i;
        uint16_t tens = 1;
        for( i = rx_idx-1 ; i>0 ; i-- )
        {
            uint8_t data = rx_buffer[i];
            if( data == '\r' || data == '\n' )
                continue;
            if( data >= '0' && data <= '9' )
            {
                data = data - '0';
                arg += data * tens;
                tens *= 10;
            }
            else
            {
                //the argument contains invalid symbols
                vcs_st = VCS_SEND_ERROR;
                return;
            }
        }
        switch( rx_buffer[0] )
        {
            case SET_LOW_VOLTAGE:
                setVoltageLow(arg);
                break;
            case SET_HIGH_VOLTAGE:
                setVoltageHigh(arg);
                break;
            case SET_DELTA_T:
                setDeltaT(arg);
                break;
        }
        vcs_st = VCS_SEND_OK;
        return;
    }
    if( vcs_st == VCS_SEND_INFO )
    {
        if( send_task_busy() == true )
            return;
        //send HIGH
        send_string("  H: ",5);
        vcs_st = VCS_SEND_INFO_1;
        return;
    }
    if( vcs_st == VCS_SEND_INFO_1 )
    {
        if( send_task_busy() == true )
            return;
        uint8_t n = num2string(num_buffer,6,getVoltageHigh());
        send_string(num_buffer,n);
        vcs_st = VCS_SEND_INFO_2;
        return;
    }
    if( vcs_st == VCS_SEND_INFO_2 )
    {
        if( send_task_busy() == true )
            return;
        send_string("\r\n",2);
        vcs_st = VCS_SEND_INFO_3;
        return;
    }
    if( vcs_st == VCS_SEND_INFO_3 )
    {
        if( send_task_busy() == true )
            return;
        //send LOW
        send_string("  L: ",5);
        vcs_st = VCS_SEND_INFO_4;
        return;
    }
    if( vcs_st == VCS_SEND_INFO_4 )
    {
        if( send_task_busy() == true )
            return;
        uint8_t n = num2string(num_buffer,6,getVoltageLow());
        send_string(num_buffer,n);
        vcs_st = VCS_SEND_INFO_5;
        return;
    }
    if( vcs_st == VCS_SEND_INFO_5 )
    {
        if( send_task_busy() == true )
            return;
        send_string("\r\n",2);
        vcs_st = VCS_SEND_INFO_6;
        return;
    }
    if( vcs_st == VCS_SEND_INFO_6 )
    {
        if( send_task_busy() == true )
            return;
        //send DeltaT
        send_string("  T: ",5);
        vcs_st = VCS_SEND_INFO_7;
        return;
    }
    if( vcs_st == VCS_SEND_INFO_7 )
    {
        if( send_task_busy() == true )
            return;
        uint8_t n = num2string(num_buffer,6,getDeltaT());
        send_string(num_buffer,n);
        vcs_st = VCS_SEND_INFO_8;
        return;
    }
    if( vcs_st == VCS_SEND_INFO_8 )
    {
        if( send_task_busy() == true )
            return;
        send_string("\r\n",2);
        vcs_st = VCS_SEND_INFO_9;
        return;
    }
    if( vcs_st == VCS_SEND_INFO_9 )
    {
        if( send_task_busy() == true )
            return;
        vcs_st = VCS_INIT;
        return;
    }
    if( vcs_st == VCS_SEND_ERROR )
    {
        if( send_task_busy() == true )
            return;
        send_string("ERROR: invalid syntax.\r\n",24);
        vcs_st = VCS_INIT;
        return;
    }
    if( vcs_st == VCS_SEND_OK )
    {
        if( send_task_busy() == true )
            return;
        send_string("OK\r\n",4);
        vcs_st = VCS_INIT;
        return;
    }
    if( vcs_st == VCS_SEND_HELP )
    {
        if( send_task_busy() == true )
            return;
        send_string("\r\n\r\n----------------\r\n",22);
        vcs_st = VCS_SEND_HELP_1;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_1 )
    {
        if( send_task_busy() == true )
            return;
        send_string("  Help Screen\r\n\r\n",17);
        vcs_st = VCS_SEND_HELP_2;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_2 )
    {
        if( send_task_busy() == true )
            return;
        send_string("Syntax: CNNNN\r\n",15);
        vcs_st = VCS_SEND_HELP_3;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_3 )
    {
        if( send_task_busy() == true )
            return;
        send_string("  C: command\r\n",14);
        vcs_st = VCS_SEND_HELP_4;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_4 )
    {
        if( send_task_busy() == true )
            return;
        send_string("  NNNN: number from 0 to 9999\r\n\r\n",33);
        vcs_st = VCS_SEND_HELP_5;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_5 )
    {
        if( send_task_busy() == true )
            return;
        send_string("Available Commands:\r\n",21);
        vcs_st = VCS_SEND_HELP_6;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_6 )
    {
        if( send_task_busy() == true )
            return;
        send_string("  L: set LOW voltage\r\n",22);
        vcs_st = VCS_SEND_HELP_7;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_7 )
    {
        if( send_task_busy() == true )
            return;
        send_string("  H: set HIGH voltage\r\n",23);
        vcs_st = VCS_SEND_HELP_8;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_8 )
    {
        if( send_task_busy() == true )
            return;
        send_string("  T: set delay TIME\r\n",21);
        vcs_st = VCS_SEND_HELP_9;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_9 )
    {
        if( send_task_busy() == true )
            return;
        send_string("  I: print system INFO\r\n",24);
        vcs_st = VCS_SEND_HELP_10;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_10 )
    {
        if( send_task_busy() == true )
            return;
        send_string("  ?: print this screen\r\n",24);
        vcs_st = VCS_SEND_HELP_11;
        return;
    }
    if( vcs_st == VCS_SEND_HELP_11 )
    {
        if( send_task_busy() == true )
            return;
        vcs_st = VCS_INIT;
        return;
    }
}

//Private methods:
static bool send_task_busy(void)
{
    return tx_string != NULL;
}

static void send_string(const char *str,uint8_t len)
{
    if( tx_string != NULL )
        return;
    
    if( str == NULL || len == 0 )
        return;
    
    tx_string = str;
    tx_len = len;
    
    vcs_sub_st = VCS_SUB_SEND_CHAR;
    tx_idx = 0;
}

static void vcs_send_task()
{
    if( vcs_sub_st == VCS_SUB_SHUTDOWN )
        return;
    if( vcs_sub_st == VCS_SUB_SEND_CHAR )
    {
        if( TX1STAbits.TRMT == 0 )
        {
            if( tx_idx == tx_len )
            {
                vcs_sub_st = VCS_SUB_SHUTDOWN;
                tx_string = NULL;
                return;
            }
            TX1REG = tx_string[tx_idx++];
        }
        return;
    }
}

static uint8_t num2string(char *str,uint8_t len,uint16_t num)
{
    int8_t ndigits;
    if( num > 9999 )
        ndigits = 5;
    else if( num > 999 )
        ndigits = 4;
    else if( num > 99 )
        ndigits = 3;
    else if( num > 9 )
        ndigits = 2;
    else
        ndigits = 1;
    ndigits--;
    
    uint8_t digits_count = 0;
    for( ; ndigits >= 0 ; ndigits-- )
    {
        if( ndigits >= len )
            continue;
        uint16_t digit = num/10;
        digit = num - digit*10;
        str[ndigits] = digit;
        digits_count++;
        if( num == 0 )
            break;
    }
    return digits_count;
}