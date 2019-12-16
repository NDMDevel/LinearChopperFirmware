#include "UartParser.h"
#include "mcc_generated_files/mcc.h"
#include "SystemTimer.h"
#include "vcontrol.h"
#include <stdbool.h>

#define RX_BUFFER_LENGTH 32
static uint8_t rx_buffer[RX_BUFFER_LENGTH];
static uint8_t rx_idx = 0;

static uint8_t isr_timer;
static bool lock_isr = true;
void EUSART_CommandHandler_ISR(void)
{
    uint8_t rx_data = RC1REG;
    if( lock_isr )
        return;
    TIMER_RESET(isr_timer);
    if( rx_idx < RX_BUFFER_LENGTH )
        rx_buffer[rx_idx++] = rx_data;
}

static enum
{
    SHUTDOWN,
    INIT,
    WAIT_IDLE,
    READ_FRAME,
    PARSE_FRAME,
    SEND_RESPONSE

}st = SHUTDOWN;

static uint8_t tx_idx;

enum Commands
{
    SET_PARAMS = 0xAA,
    GET_PARAMS = 0x55,
    GET_VDC = 0xA5
};

void uart_start(void)
{
    if( st == SHUTDOWN )
    {
        TMR1_SetInterruptHandler(TMR1_SystemTimer_ISR);
        EUSART_SetRxInterruptHandler(EUSART_CommandHandler_ISR);
        st = INIT;
    }
}

void uart_stop(void)
{
    TMR1_SetInterruptHandler(TMR1_DefaultInterruptHandler);
    EUSART_SetRxInterruptHandler(EUSART_Receive_ISR);
    st = SHUTDOWN;
}

void uart_task(void)
{
    if( st == SHUTDOWN )
        return;
    if( st == INIT )
    {
        TIMER_RESET(isr_timer);
        lock_isr = false;
        st = WAIT_IDLE;
        return;
    }
    if( st == WAIT_IDLE )
    {
        if( TIMER_ELAPSE(isr_timer,t300ms) == false )
            return;
        rx_idx = 0;
        st = READ_FRAME;
        return;
    }
    if( st == READ_FRAME )
    {
        if( rx_idx == 0 )
            return;         //no data recived
        if( TIMER_ELAPSE(isr_timer,t300ms) == false )
            return;         //no 300ms of silence in the channel

        lock_isr = true;
        
        uint8_t i;
        uint8_t xor = rx_buffer[0];
        for( i=1 ; i<rx_idx ; i++ )
            xor ^= rx_buffer[i];
        if( xor != 0                                                    ||
            (rx_buffer[0] != SET_PARAMS  && rx_buffer[0] != GET_PARAMS && rx_buffer[0] != GET_VDC) ||
            (rx_idx != 11 && rx_idx != 2)                               )
        {
            rx_idx = 0; //stay in this state
            lock_isr = false;
        }
        else
            st = PARSE_FRAME;
        return;
    }
    if( st == PARSE_FRAME )
    {
        if( rx_buffer[0] == SET_PARAMS && rx_idx == 11 )
        {
            uint16_t val;
            val = rx_buffer[1];
            val = val<<8 | rx_buffer[2];
            set_vdc_min(val);
            val = rx_buffer[3];
            val = val<<8 | rx_buffer[4];
            set_vdc_max(val);
            val = rx_buffer[5];
            val = val<<8 | rx_buffer[6];
            set_vdc_critic(val);
            val = rx_buffer[7];
            val = val<<8 | rx_buffer[8];
            set_vdc_speed(val);
            if( rx_buffer[9] == 0 )
                stop_chopper();
            else
                start_chopper();
            save_to_flash();
            st = INIT;
            return;
        }
        else
        {
            if( rx_buffer[0] == GET_PARAMS && rx_idx == 2 )
            {
                uint16_t val;
                val = get_vdc_min();
                rx_buffer[1] = (val>>8) & 0xFF;
                rx_buffer[2] = val & 0xFF;
                val = get_vdc_max();
                rx_buffer[3] = (val>>8) & 0xFF;
                rx_buffer[4] = val & 0xFF;
                val = get_vdc_critic();
                rx_buffer[5] = (val>>8) & 0xFF;
                rx_buffer[6] = val & 0xFF;
                val = get_vdc_speed();
                rx_buffer[7] = (val>>8) & 0xFF;
                rx_buffer[8] = val & 0xFF;
                rx_buffer[9] = is_chopper_active();
                uint8_t i;
                uint8_t xor = rx_buffer[0];
                for( i=1 ; i<10 ; i++ )
                    xor ^= rx_buffer[i];
                rx_buffer[10] = xor;
                rx_idx = 11;
                tx_idx = 0;
                st = SEND_RESPONSE;
            }
            else
            {
                if( rx_buffer[0] == GET_VDC && rx_idx == 2 )
                {
                    uint16_t val;
                    val = get_vdc();
                    rx_buffer[1] = (val>>8) & 0xFF;
                    rx_buffer[2] = val & 0xFF;
                    rx_buffer[3] = rx_buffer[0] ^ rx_buffer[1] ^ rx_buffer[2];
                    rx_idx = 4;
                    tx_idx = 0;
                    st = SEND_RESPONSE;
                }
                else
                    //error, command not valid
                    st = INIT;
            }
        }
        return;
    }
    if( st == SEND_RESPONSE )
    {
        if( TX1STAbits.TRMT == false )
            return;
        if( tx_idx < rx_idx )
            TX1REG = rx_buffer[tx_idx++];
        else
            st = INIT;
        return;
    }
}
