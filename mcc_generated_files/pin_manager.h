/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.78
        Device            :  PIC16F1705
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB 	          :  MPLAB X 5.20	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set RA2 procedures
#define RA2_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define RA2_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define RA2_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define RA2_GetValue()              PORTAbits.RA2
#define RA2_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define RA2_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define RA2_SetPullup()             do { WPUAbits.WPUA2 = 1; } while(0)
#define RA2_ResetPullup()           do { WPUAbits.WPUA2 = 0; } while(0)
#define RA2_SetAnalogMode()         do { ANSELAbits.ANSA2 = 1; } while(0)
#define RA2_SetDigitalMode()        do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set LED_R aliases
#define LED_R_TRIS                 TRISAbits.TRISA4
#define LED_R_LAT                  LATAbits.LATA4
#define LED_R_PORT                 PORTAbits.RA4
#define LED_R_WPU                  WPUAbits.WPUA4
#define LED_R_OD                   ODCONAbits.ODA4
#define LED_R_ANS                  ANSELAbits.ANSA4
#define LED_R_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define LED_R_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define LED_R_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define LED_R_GetValue()           PORTAbits.RA4
#define LED_R_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define LED_R_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define LED_R_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define LED_R_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define LED_R_SetPushPull()        do { ODCONAbits.ODA4 = 0; } while(0)
#define LED_R_SetOpenDrain()       do { ODCONAbits.ODA4 = 1; } while(0)
#define LED_R_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define LED_R_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set LED_G aliases
#define LED_G_TRIS                 TRISAbits.TRISA5
#define LED_G_LAT                  LATAbits.LATA5
#define LED_G_PORT                 PORTAbits.RA5
#define LED_G_WPU                  WPUAbits.WPUA5
#define LED_G_OD                   ODCONAbits.ODA5
#define LED_G_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define LED_G_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define LED_G_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define LED_G_GetValue()           PORTAbits.RA5
#define LED_G_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define LED_G_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define LED_G_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define LED_G_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define LED_G_SetPushPull()        do { ODCONAbits.ODA5 = 0; } while(0)
#define LED_G_SetOpenDrain()       do { ODCONAbits.ODA5 = 1; } while(0)

// get/set RC0 procedures
#define RC0_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define RC0_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define RC0_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define RC0_GetValue()              PORTCbits.RC0
#define RC0_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define RC0_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define RC0_SetPullup()             do { WPUCbits.WPUC0 = 1; } while(0)
#define RC0_ResetPullup()           do { WPUCbits.WPUC0 = 0; } while(0)
#define RC0_SetAnalogMode()         do { ANSELCbits.ANSC0 = 1; } while(0)
#define RC0_SetDigitalMode()        do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set RC1 procedures
#define RC1_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define RC1_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define RC1_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define RC1_GetValue()              PORTCbits.RC1
#define RC1_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define RC1_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define RC1_SetPullup()             do { WPUCbits.WPUC1 = 1; } while(0)
#define RC1_ResetPullup()           do { WPUCbits.WPUC1 = 0; } while(0)
#define RC1_SetAnalogMode()         do { ANSELCbits.ANSC1 = 1; } while(0)
#define RC1_SetDigitalMode()        do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set RC2 procedures
#define RC2_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define RC2_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define RC2_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define RC2_GetValue()              PORTCbits.RC2
#define RC2_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define RC2_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define RC2_SetPullup()             do { WPUCbits.WPUC2 = 1; } while(0)
#define RC2_ResetPullup()           do { WPUCbits.WPUC2 = 0; } while(0)
#define RC2_SetAnalogMode()         do { ANSELCbits.ANSC2 = 1; } while(0)
#define RC2_SetDigitalMode()        do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set V_BUSDC aliases
#define V_BUSDC_TRIS                 TRISCbits.TRISC3
#define V_BUSDC_LAT                  LATCbits.LATC3
#define V_BUSDC_PORT                 PORTCbits.RC3
#define V_BUSDC_WPU                  WPUCbits.WPUC3
#define V_BUSDC_OD                   ODCONCbits.ODC3
#define V_BUSDC_ANS                  ANSELCbits.ANSC3
#define V_BUSDC_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define V_BUSDC_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define V_BUSDC_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define V_BUSDC_GetValue()           PORTCbits.RC3
#define V_BUSDC_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define V_BUSDC_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define V_BUSDC_SetPullup()          do { WPUCbits.WPUC3 = 1; } while(0)
#define V_BUSDC_ResetPullup()        do { WPUCbits.WPUC3 = 0; } while(0)
#define V_BUSDC_SetPushPull()        do { ODCONCbits.ODC3 = 0; } while(0)
#define V_BUSDC_SetOpenDrain()       do { ODCONCbits.ODC3 = 1; } while(0)
#define V_BUSDC_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define V_BUSDC_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set RELAY_GPIO aliases
#define RELAY_GPIO_TRIS                 TRISCbits.TRISC5
#define RELAY_GPIO_LAT                  LATCbits.LATC5
#define RELAY_GPIO_PORT                 PORTCbits.RC5
#define RELAY_GPIO_WPU                  WPUCbits.WPUC5
#define RELAY_GPIO_OD                   ODCONCbits.ODC5
#define RELAY_GPIO_ANS                  ANSELCbits.ANSC5
#define RELAY_GPIO_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define RELAY_GPIO_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define RELAY_GPIO_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RELAY_GPIO_GetValue()           PORTCbits.RC5
#define RELAY_GPIO_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define RELAY_GPIO_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define RELAY_GPIO_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define RELAY_GPIO_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define RELAY_GPIO_SetPushPull()        do { ODCONCbits.ODC5 = 0; } while(0)
#define RELAY_GPIO_SetOpenDrain()       do { ODCONCbits.ODC5 = 1; } while(0)
#define RELAY_GPIO_SetAnalogMode()      do { ANSELCbits.ANSC5 = 1; } while(0)
#define RELAY_GPIO_SetDigitalMode()     do { ANSELCbits.ANSC5 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/