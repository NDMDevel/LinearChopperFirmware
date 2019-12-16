#ifndef VCOMSHELL_H
#define	VCOMSHELL_H

#define RX_BUFFER_LENGTH 16
#define TX_BUFFER_LENGTH 32
#define END_OF_COMMAND_CHAR 13

enum
{
    SET_LOW_VOLTAGE     = 'L',
    SET_HIGH_VOLTAGE    = 'H',
    SET_DELTA_T         = 'T',
    GET_INFO            = 'I',
    GET_HELP            = '?'

}Commands;

void vcs_task(void);
void vcs_start(void);
void vcs_stop(void);

void uart_vcs_isr(void);


#endif	/* VCOMSHELL_H */

