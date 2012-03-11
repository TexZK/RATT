/*********************************************************************
 *
 *                Microchip USB C18 Firmware 
 *
 *********************************************************************
 * FileName:        io_cfg.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 3.11+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * File Version  Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 1.0			 04/09/2008	Started from MCHPFSUSB v1.3 HID Mouse
 *							demo project.  Commented out items that
 *							are not particularly useful for the
 *							bootloader.
 ********************************************************************/

#ifndef IO_CFG_H
#define IO_CFG_H

/** I N C L U D E S *************************************************/
#include "usbcfg.h"
#include "../../firmware/include/app.h"
#include "../../firmware/include/leds.h"

/** T R I S *********************************************************/
#define INPUT_PIN           1
#define OUTPUT_PIN          0

//Uncomment the below line(s) if the hardware supports it and
//it is desireable to use one or both of the features.
//#define USE_SELF_POWER_SENSE_IO	
//#define USE_USB_BUS_SENSE_IO

#define tris_usb_bus_sense  TRISAbits.TRISA1    // Input


#if defined(USE_USB_BUS_SENSE_IO)
#define usb_bus_sense       PORTAbits.RA1
#else
#define usb_bus_sense       1
#endif

#define tris_self_power     TRISAbits.TRISA2    // Input

#if defined(USE_SELF_POWER_SENSE_IO)
#define self_power          PORTAbits.RA2
#else
#define self_power          1
#endif


/** LED ************************************************************/
#define mInitAllLEDs()      { LEDS_TRIS_GREEN=0; LEDS_TRIS_YELLOW=0; LEDS_TRIS_RED=0; }

#define mLED_1              GREEN_LED
#define mLED_2              YELLOW_LED
#define mLED_3              RED_LED
    
#define mLED_1_On()         mLED_1 = LED_ON;
#define mLED_2_On()         mLED_2 = LED_ON;
#define mLED_3_On()         mLED_3 = LED_ON;

#define mLED_1_Off()        mLED_1 = LED_OFF;
#define mLED_2_Off()        mLED_2 = LED_OFF;
#define mLED_3_Off()        mLED_3 = LED_OFF;

#define mLED_1_Toggle()     mLED_1 = !mLED_1;
#define mLED_2_Toggle()     mLED_2 = !mLED_2;
#define mLED_3_Toggle()     mLED_3 = !mLED_3;

/** SWITCH *********************************************************/
#define sw2                 BUTTON_PIN
#define sw3                 1
#define mInitAllSwitches()


#endif //IO_CFG_H
