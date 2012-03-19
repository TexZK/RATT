/*
 * FileName:		main.c
 * Dependencies:	See #include definitions
 * Processor:		PIC18F14K50
 * Hardware:		MoSe board
 * Complier:		Microchip C18
 * Company:		Andrea "TexZK" Zoppi
 * 
 * Software License Agreement:
 * 
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC Microcontroller products. The
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
 */

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS

#include "Compiler.h"
#include "TimeDelay.h"

#include "usb/usb.h"
#include "usb/usb_function_hid.h"
#include "usb/usb_user.h"

#include "app.h"
#include "bootloader.h"
#include "adns2080.h"
#include "leds.h"
#include "debug.h"
#include "incenc.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CONFIGURATION

#pragma	config	CPUDIV	= NOCLKDIV
#pragma	config	USBDIV	= OFF
#pragma	config	FOSC	= HS
#pragma	config	PLLEN	= ON
#pragma	config	FCMEN	= OFF
#pragma	config	IESO	= OFF
#pragma	config	PWRTEN	= ON
#pragma	config	BOREN	= ON
#pragma	config	BORV	= 30
#pragma	config	WDTEN	= OFF
#pragma	config	WDTPS	= 32768
#pragma	config	MCLRE	= OFF
#pragma	config	HFOFST	= OFF
#pragma	config	STVREN	= ON
#pragma	config	LVP		= OFF
#pragma	config	XINST	= OFF
#pragma	config	BBSIZ	= OFF
#pragma	config	CP0		= OFF
#pragma	config	CP1		= OFF
#pragma	config	CPB		= OFF
#pragma	config	WRT0	= OFF
#pragma	config	WRT1	= OFF
#pragma	config	WRTB	= OFF
#pragma	config	WRTC	= OFF
#pragma	config	EBTR0	= OFF
#pragma	config	EBTR1	= OFF
#pragma	config	EBTRB	= OFF


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_app_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata data_app_global


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL PROTOTYPES
#pragma code code_app_local

/**
 * Initializes ports and modules.
 */
void InitializeSystem( void );


/**
 * Background process.
 */
void ProcessIO( void );


/**
 * High priority ISR, fast call.
 */
void HighPriorityISR( void );


// ISR
#pragma code code_isr

/**
 * Low priority ISR, standard call.
 */
void LowPriorityISR( void );


// USB callbacks
#pragma code code_app_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// VECTOR REMAPPING (for HID bootloader)

#pragma code
extern void _startup ( void );	// See c018i.c in your C18 compiler dir
#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
void _reset( void )
{
    _asm goto _startup _endasm
}

#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void Remapped_High_ISR( void )
{
     _asm goto HighPriorityISR _endasm
}

#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
void Remapped_Low_ISR( void )
{
     _asm goto LowPriorityISR _endasm
}

#pragma code HIGH_INTERRUPT_VECTOR = 0x08
void High_ISR( void )
{
     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
}

#pragma code LOW_INTERRUPT_VECTOR = 0x18
void Low_ISR( void )
{
     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// INTERRUPTS

#pragma code code_isr

#pragma interrupt HighPriorityISR
void HighPriorityISR( void )
{	
	// Check for incremental encoder motion
	// Note: Mutually exclusive thanks to the Gray code
	if ( INCENC_INT_IE_A ) {
		if ( INCENC_INT_IF_A ) {
			IncEnc_ChangeCallback();
		}
	}
	if ( INCENC_INT_IE_B ) {
		if ( INCENC_INT_IF_B ) {
			IncEnc_ChangeCallback();
		}
	}
	
	// Check if the motion sensor has detected motion
	if ( ADNS_INT_IE ) {
		if ( ADNS_INT_IF ) {
			Adns_MotionCallback();
		}
	}
}

#pragma interruptlow LowPriorityISR
void LowPriorityISR( void )
{
	// Process USB tasks
	#ifdef USB_INTERRUPT
	USBDeviceTasks();
	#endif
	
	// Process debug UART tasks
	if ( DEBUG_UART_INT_RX ) {
		if ( DEBUG_UART_FLAG_RX ) {
			Debug_RxIntCallback();
		}
	}
	if ( DEBUG_UART_INT_TX ) {
		if ( DEBUG_UART_FLAG_TX ) {
			Debug_TxIntCallback();
		}
	}
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL FUNCTIONS
#pragma code code_app_local

/**
 * Centralized initialization routine.
 */
void InitializeSystem( void )
{
    INTCON2bits.NOT_RABPU = 1;		// All pull-ups off
    
    INTCON = 0;						// Disable all interrupts
    INTCON3 = 0;
    PIE1 = 0;
    PIE1 = 0;
    PIR1 = 0;						// Clear peripheral flags
    PIR2 = 0;
    RCONbits.IPEN = 1;				// Use interrupt priority
    
    // All pins as inputs
    TRISA = ~0;
    TRISB = ~0;
    TRISC = ~0;
    
    // Configure analog pins
    ANSEL = INCENC_ANSBM_A | INCENC_ANSBM_B;
    ANSELH = AUXIN_ANSHBM;
    
    // Initialize modules
    Leds_Initialize();				// Initialize the LEDs module
    YELLOW_LED = LED_ON;			// Yellow LED to show initialization
    Debug_Initialize();				// Initialize the debug module; NOTE: enables HP&LP interrupts!
    Usb_UserInit();					// Initialize USB module
    Adns_Initialize();				// Initialize ADNS module
    IncEnc_Initialize();			// Initialize the incremental encoder module
    
    Debug_PrintConst_NewLine();
    Debug_PrintConst_NewLine();
    Debug_PrintConst_Dots();
    Debug_PrintRom_( "INIT" );
    Debug_PrintConst_Ok();
    Debug_PrintConst_NewLine();
    Debug_PrintConst_NewLine();
    Debug_Flush();
    
    GREEN_LED = LED_ON;				// Green LED for initialization completed
    YELLOW_LED = LED_OFF;
    RED_LED = LED_OFF;
    
    #ifdef USB_INTERRUPT
    // Attach USB device
    USBDeviceAttach();
    Debug_PrintConst_NewLine();
    Debug_PrintConst_NewLine();
    Debug_Flush();
    #endif
}


/**
 * This function is a place holder for other user routines. It is a mixture of
 * both USB and non-USB tasks.
 */
void ProcessIO( void )
{
	Debug_Flush();					// Flush the debug console, to speed-up further messages
	
	// User Application USB tasks
    if ( USBDeviceState < CONFIGURED_STATE || USBSuspendControl == 1 ) {
		return;
	}
    
    // Process modules
    IncEnc_Service();				// Handle incremental encoder tasks
    Adns_Service();					// Handle ADNS tasks
	
	// TODO: Build & transfer HID packets here and not in ADNS service
	
	// Check if data was received from the host
    if ( Usb_RxReady() ) {
		// TODO: Process ReceivedDataBuffer[*]
        
		// Re-arm the OUT endpoint for the next packet
		Usb_RxBufferedPacket();
	}	
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL FUNCTIONS
#pragma code code_app_global

/**
 * Program entry point
 */
void main( void )
{   
    InitializeSystem();

    while ( 1 ) {
        #ifdef USB_POLLING
		/* Check bus status and service USB interrupts.
         * Interrupt or polling method. If using polling, must call
         * this function periodically. This function will take care
         * of processing and responding to SETUP transactions 
         * (such as during the enumeration process when you first
         * plug in). USB hosts require that USB devices should accept
         * and process SETUP packets in a timely fashion. Therefore,
         * when using polling, this function should be called 
         * regularly (such as once every 1.8ms or faster** [see 
         * inline code comments in usb_device.c for explanation when
         * "or faster" applies])  In most cases, the USBDeviceTasks() 
         * function does not take very long to execute (ex: <100 
         * instruction cycles) before it returns.
         */
		USBDeviceTasks();
		#endif
    	
    	ProcessIO();        
    }
}


void App_WaitButtonPress( void )
{
	do {
		DelayMs( 10 );			// Debounce previous release
	} while ( !BUTTON_PIN );	// Wait for previous release
	while ( BUTTON_PIN ) {		// Wait for button press
		DelayMs( 10 );			// Debounce button press
	}
}


void OneUsDelay( void )
{
	// (R)CALL = 2 NOPs
	Nop();	// 8 NOPs
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	// RETURN = 2 NOPs
}

