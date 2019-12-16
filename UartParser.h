/* 
 * File:   UartParser.h
 * Author: Damian
 *
 * Created on December 12, 2019, 8:34 PM
 */

#ifndef UARTPARSER_H
#define	UARTPARSER_H

void EUSART_CommandHandler_ISR(void);
void uart_start(void);
void uart_stop(void);
void uart_task(void);


#endif	/* UARTPARSER_H */

