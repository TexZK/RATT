/*
 FileName:		main.c
 Dependencies:	See #include definitions
 Processor:		PIC18F14K50
 Hardware:		RATT board
 Complier:		Microchip C18
 Company:		Andrea Zoppi

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company”) for its PIC® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
  Rev   Description
  ----  -----------------------------------------
  1.0   Initial release
*/

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS

#include "HardwareProfile.h"
#include "usb/usb.h"
#include "usb/usb_function_hid.h"
#include "adns2080.h"
#include "leds.h"


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
#pragma udata data_main_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata usbram2

// USB buffers
unsigned char usb_txBuffer[64];
unsigned char usb_rxBuffer[64];

#pragma udata data_main_global

USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
BOOL blinkStatusValid = TRUE;


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL PROTOTYPES

void InitializeSystem( void );
void ProcessIO( void );
void UserInit( void );
void HighPriorityISR( void );
void LowPriorityISR( void );
void USBCBSendResume( void );


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// VECTOR REMAPPING (for HID bootloader)

#define	REMAPPED_RESET_VECTOR_ADDRESS			0x1000
#define	REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
#define	REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018

#pragma code
extern void _startup ( void );	// See c018i.c in your C18 compiler dir
#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
void _reset( void )
{
    _asm goto _startup _endasm
}

#pragma code
#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void Remapped_High_ISR( void )
{
     _asm goto HighPriorityISR _endasm
}

#pragma code
#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
void Remapped_Low_ISR( void )
{
     _asm goto LowPriorityISR _endasm
}

#pragma code
#pragma code HIGH_INTERRUPT_VECTOR = 0x08
void High_ISR( void )
{
     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
}

#pragma code
#pragma code LOW_INTERRUPT_VECTOR = 0x18
void Low_ISR( void )
{
     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
}

#pragma code

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// INTERRUPTS

#pragma code code_isr

#pragma interrupt HighPriorityISR
void HighPriorityISR( void )
{
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
	/*if ( DEBUG_UART_RX_INT ) {
		if ( DEBUG_UART_RX_FLAG ) {
			Debug_RxIntCallback();
		}
	}
	if ( DEBUG_UART_TX_INT ) {
		if ( DEBUG_UART_TX_FLAG ) {
			Debug_TxIntCallback();
		}
	}*/
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL FUNCTIONS
#pragma code code_main_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL FUNCTIONS
#pragma code code_main_global

/**
 * Program entry point
 */
void main( void )
{   
    InitializeSystem();

    #ifdef USB_INTERRUPT
    USBDeviceAttach();
    #endif

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


/**
 * InitializeSystem is a centralize initialization routine. All required USB
 * initialization routines are called from here.
 *
 * User application initialization routine should also be called from here.
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
    
    // Setup pins
    TRISA = ~0;						// All inputs
    TRISB = ~0;
    TRISC = ~0;
    
    // Initialize the debug module
//    Debug_Initialize();
//    INTCONbits.GIEH = 1;
//    INTCONbits.GIEL = 1;
//    Debug_PrintRom( (const far rom char *)"=> RATT <=\nby TexZK\n\n" );
//    Debug_Flush();
    
    // Initialize USB module
//    Debug_PrintRom( (const far rom char *)"Init USB\n" );
    USBOutHandle = 0;
    USBInHandle = 0;
    USBDeviceInit();
//    Debug_Flush();
    
    // TODO: Initialize ADNS module
//    Debug_PrintRom( (const far rom char *)"Init ADNS\n" );
	Adns_Initialize();
//    Debug_Flush();
    
    // Initialize the LEDs module
//    Debug_PrintRom( (const far rom char *)"Init LEDs\n" );
    Leds_Initialize();
//    Debug_Flush();
    
//    Debug_PrintRom( (const far rom char *)"\nInit OK!\n\n" );
//    Debug_Flush();
}


/**
 * This function is a place holder for other user routines. It is a mixture of
 * both USB and non-USB tasks.
 */
void ProcessIO( void )
{   
	// TODO: Handle ADNS
	
	// User Application USB tasks
    if ( USBDeviceState < CONFIGURED_STATE || USBSuspendControl == 1 ) {
		return;
	}
    
    // Check if data was received from the host
    if ( !HIDRxHandleBusy( USBOutHandle ) ) {
		// TODO: Process ReceivedDataBuffer[*]
        
		// Re-arm the OUT endpoint for the next packet
		USBRxBufferedPacket();
	}	
}


void USBTxBufferedPacket( void )
{
	USBInHandle = HIDTxPacket( HID_EP, (BYTE *)&usb_txBuffer[0], sizeof(usb_txBuffer) );
}


void USBRxBufferedPacket( void )
{
	USBOutHandle = HIDRxPacket( HID_EP, (BYTE *)&usb_rxBuffer[0], sizeof(usb_rxBuffer) );
}


unsigned char USBTxReady( void )
{
	return !HIDTxHandleBusy( USBInHandle );
}


unsigned char USBRxReady( void )
{
	return !HIDRxHandleBusy( USBOutHandle );
}		


void OneUsDelay( void )
{
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// USB CALLBACK FUNCTIONS

/*
 * The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
 * events. For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
 * packets to your device. In response to this, all USB devices are supposed to decrease their power
 * consumption from the USB Vbus to <2.5mA each. The USB module detects this condition (which according
 * to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
 * function. You should modify these callback functions to take appropriate actions for each of these
 * conditions. For example, in the USBCBSuspend(), you may wish to add code that will decrease power
 * consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
 * microcontroller to sleep, etc.). Then, in the USBCBWakeFromSuspend() function, you may then wish to
 * add code that undoes the power saving things done in the USBCBSuspend() function.
 * 
 * The USBCBSendResume() function is special, in that the USB stack will not automatically call this
 * function. This function is meant to be called from the application firmware instead. See the
 * additional comments near the function.
 */

/**
 * Call back that is invoked when a USB suspend is detected
 */
void USBCBSuspend( void )
{
	/* 
	 * Example power saving code. Insert appropriate code here for the desired
	 * application behavior. If the microcontroller will be put to sleep, a
	 * process similar to that shown below may be used:
	 * 
	 * ConfigureIOPinsForLowPower();
	 * SaveStateOfAllInterruptEnableBits();
	 * DisableAllInterruptEnableBits();
	 * EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
	 * Sleep();
	 * RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	 * RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	 * 
	 * IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here. This bit is 
	 * cleared inside the usb_device.c file. Clearing USBActivityIF here will cause 
	 * things to not work as intended.	
	 */
}


/**
 * The host may put USB peripheral devices in low power suspend mode (by
 * "sending" 3+ms of idle).
 * Once in suspend mode, the host may wake the device back up by sending non-
 * idle state signalling.
 * 
 * This call back is invoked when a wakeup from USB suspend is detected.
 */
void USBCBWakeFromSuspend( void )
{
	/*
	 * If clock switching or other power savings measures were taken when
	 * executing the USBCBSuspend() function, now would be a good time to
	 * switch back to normal full power run mode conditions. The host allows
	 * a few milliseconds of wakeup time, after which the device must be 
	 * fully back to normal, and capable of receiving and processing USB
	 * packets. In order to do this, the USB module must receive proper
	 * clocking (IE: 48MHz clock must be available to SIE for full speed USB
	 * operation).
	 */
}


/**
 * The USB host sends out a SOF packet to full-speed devices every 1 ms. This
 * interrupt may be useful for isochronous pipes. End designers should
 * implement callback routine as necessary.
 */
void USBCB_SOF_Handler( void )
{
    /*
     * No need to clear UIRbits.SOFIF to 0 here. Callback caller is already
     * doing that.
     */
}


/**
 * The purpose of this callback is mainly for debugging during development.
 * Check UEIR to see which error causes the interrupt.
 */
void USBCBErrorHandler( void )
{
    /*
     * No need to clear UEIR to 0 here.
     * Callback caller is already doing that.
	 * 
	 * Typically, user firmware does not need to do anything special
	 * if a USB error occurs. For example, if the host sends an OUT
	 * packet to your device, but the packet gets corrupted (ex:
	 * because of a bad connection, or the user unplugs the
	 * USB cable during the transmission) this will typically set
	 * one or more USB error interrupt flags. Nothing specific
	 * needs to be done however, since the SIE will automatically
	 * send a "NAK" packet to the host. In response to this, the
	 * host will normally retry to send the packet again, and no
	 * data loss occurs. The system will typically recover
	 * automatically, without the need for application firmware
	 * intervention.
	 * 
	 * Nevertheless, this callback function is provided, such as
	 * for debugging purposes.
	 */
}


/**
 * When SETUP packets arrive from the host, some
 * firmware must process the request and respond
 * appropriately to fulfill the request. Some of
 * the SETUP packets will be for standard
 * USB "chapter 9" (as in, fulfilling chapter 9 of
 * the official USB specifications) requests, while
 * others may be specific to the USB device class
 * that is being implemented. For example, a HID
 * class device needs to be able to respond to
 * "GET REPORT" type of requests. This
 * is not a standard USB chapter 9 request, and 
 * therefore not handled by usb_device.c. Instead
 * this request should be handled by class specific 
 * firmware, such as that contained in usb_function_hid.c.
 */
void USBCBCheckOtherReq( void )
{
    USBCheckHIDRequest();
}


/**
 * The USBCBStdSetDscHandler() callback function is
 * called when a SETUP, bRequest: SET_DESCRIPTOR request
 * arrives. Typically SET_DESCRIPTOR requests are
 * not used in most applications, and it is
 * optional to support this type of request.
 */
void USBCBStdSetDscHandler( void )
{
    // Must claim session ownership if supporting this request
}


/**
 * This function is called when the device becomes
 * initialized, which occurs after the host sends a
 * SET_CONFIGURATION (wValue not = 0) request. This 
 * callback function should initialize the endpoints 
 * for the device's usage according to the current 
 * configuration.
 */
void USBCBInitEP( void )
{
    // Enable the HID endpoint
    USBEnableEndpoint( HID_EP, USB_IN_ENABLED | USB_OUT_ENABLED | USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP );
    // Re-arm the OUT endpoint for the next packet
    USBRxBufferedPacket();
}


/**
 * The USB specifications allow some types of USB
 * peripheral devices to wake up a host PC (such
 * as if it is in a low power suspend to RAM state).
 * This can be a very useful feature in some
 * USB applications, such as an Infrared remote
 * control	receiver. If a user presses the "power"
 * button on a remote control, it is nice that the
 * IR receiver can detect this signalling, and then
 * send a USB "command" to the PC to wake up.
 * 
 * The USBCBSendResume() "callback" function is used
 * to send this special USB signalling which wakes 
 * up the PC. This function may be called by
 * application firmware to wake up the PC. This
 * function will only be able to wake up the host if
 * all of the below are true:
 *	
 * 1. The USB driver used on the host PC supports
 *    the remote wakeup capability.
 * 2. The USB configuration descriptor indicates
 *    the device is remote wakeup capable in the
 *    bmAttributes field.
 * 3. The USB host PC is currently sleeping,
 *    and has previously sent your device a SET 
 *    FEATURE setup packet which "armed" the
 *    remote wakeup capability.  
 *
 * If the host has not armed the device to perform remote wakeup,
 * then this function will return without actually performing a
 * remote wakeup sequence. This is the required behavior, 
 * as a USB device that has not been armed to perform remote 
 * wakeup must not drive remote wakeup signalling onto the bus;
 * doing so will cause USB compliance testing failure.
 * 
 * This callback should send a RESUME signal that
 * has the period of 1-15ms.
 * 
 * Note: This function does nothing and returns quickly, if the USB
 * bus and host are not in a suspended condition, or are 
 * otherwise not in a remote wakeup ready state. Therefore, it
 * is safe to optionally call this function regularly, ex: 
 * anytime application stimulus occurs, as the function will
 * have no effect, until the bus really is in a state ready
 * to accept remote wakeup. 
 * 
 * When this function executes, it may perform clock switching,
 * depending upon the application specific code in 
 * USBCBWakeFromSuspend(). This is needed, since the USB
 * bus will no longer be suspended by the time this function
 * returns. Therefore, the USB module will need to be ready
 * to receive traffic from the host.
 * 
 * The modifiable section in this routine may be changed
 * to meet the application needs. Current implementation
 * temporary blocks other functions from executing for a
 * period of ~3-15 ms depending on the core frequency.
 * 
 * According to USB 2.0 specification section 7.1.7.7,
 * "The remote wakeup device must hold the resume signaling
 * for at least 1 ms but for no more than 15 ms."
 * The idea here is to use a delay counter loop, using a
 * common value that would work over a wide range of core
 * frequencies.
 * That value selected is 1800. See table below:
 * ==========================================================
 * Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 * ==========================================================
 *     48              12          1.05
 *      4              1           12.6
 * ==========================================================
 * * These timing could be incorrect when using code
 * optimization or extended instruction mode,
 * or when having other interrupts enabled.
 * Make sure to verify using the MPLAB SIM's Stopwatch
 * and verify the actual signal on an oscilloscope.
 */
void USBCBSendResume( void )
{
    static WORD delay_count;
    
    /*
	 * First verify that the host has armed us to perform remote wakeup.
     * It does this by sending a SET_FEATURE request to enable remote wakeup,
     * usually just before the host goes to standby mode (note: it will only
     * send this SET_FEATURE request if the configuration descriptor declares
     * the device as remote wakeup capable, AND, if the feature is enabled
     * on the host (ex: on Windows based hosts, in the device manager 
     * properties page for the USB device, power management tab, the 
     * "Allow this device to bring the computer out of standby." checkbox 
     * should be checked).
     */
    if ( USBGetRemoteWakeupStatus() == TRUE ) {
        // Verify that the USB bus is in fact suspended, before we send
        // remote wakeup signalling.
        if ( USBIsBusSuspended() == TRUE ) {
            USBMaskInterrupts();
            
            // Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FALSE;  //So we don't execute this code again until a new suspend condition is detected.
            
			/*
             * Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
             * device must continuously see 5ms+ of idle on the bus, before it sends
             * remote wakeup signalling. One way to be certain that this parameter
             * gets met, is to add a 2ms+ blocking delay here (2ms plus at 
             * least 3ms from bus idle to USBIsBusSuspended() == TRUE, yeilds
             * 5ms+ total delay since start of idle).
             */
            delay_count = 3600U;        
            do {
                --delay_count;
            } while ( delay_count );
            
            // Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1;	// Start RESUME signaling
            delay_count = 1800U;	// Set RESUME line for 1-13 ms
            do {
				--delay_count;
            } while ( delay_count );
            USBResumeControl = 0;	//Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}


/**
 * This function is called from the USB stack to
 * notify a user application that a USB event
 * occured. This callback is in interrupt context
 * when the USB_INTERRUPT option is selected.
 *
 * @param event
 *		Type of the event.
 * @param pdata
 *		Pointer to the event data.
 * @param size
 *		Size of the event data.
 *
 * @return
 *		Event handled.
 */
BOOL USER_USB_CALLBACK_EVENT_HANDLER( USB_EVENT event, void * pdata, WORD size )
{
    switch ( event )
    {
        case EVENT_TRANSFER: {
            // Add application specific callback task or callback function here if desired.
            break;
		}
        case EVENT_SOF: {
            USBCB_SOF_Handler();
            break;
		}
        case EVENT_SUSPEND: {
            USBCBSuspend();
            break;
		}
        case EVENT_RESUME: {
            USBCBWakeFromSuspend();
            break;
		}
        case EVENT_CONFIGURED: {
            USBCBInitEP();
            break;
		}
        case EVENT_SET_DESCRIPTOR: {
            USBCBStdSetDscHandler();
            break;
		}
        case EVENT_EP0_REQUEST: {
            USBCBCheckOtherReq();
            break;
		}
        case EVENT_BUS_ERROR: {
            USBCBErrorHandler();
            break;
		}
        case EVENT_TRANSFER_TERMINATED: {
            /*
             * Add application specific callback task or callback function here if desired.
             * The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
             * FEATURE (endpoint halt) request on an application endpoint which was 
             * previously armed (UOWN was = 1). Here would be a good place to:
             * 1. Determine which endpoint the transaction that just got terminated was 
             *    on, by checking the handle value in the *pdata.
             * 2. Re-arm the endpoint if desired (typically would be the case for OUT 
             *    endpoints).
             */
            break;
		}
        default: {
            break;
		}
    }      
    return TRUE; 
}

