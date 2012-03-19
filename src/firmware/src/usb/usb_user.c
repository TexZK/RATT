/**
 * User USB functions.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CONFIGURATION

//#ifndef	__DEBUG
#define	DONT_USE_DEBUG_CONSOLE
//#endif


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS

#include "usb/usb_user.h"
#include "app.h"
#include "debug.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_usbuser_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata usbram2

// USB buffers
unsigned char	usb_txBuffer[64];
unsigned char	usb_rxBuffer[64];


#pragma udata data_sys_global

// Handles
USB_HANDLE		usb_outHandle;
USB_HANDLE		usb_inHandle;


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL PROTOTYPES
#pragma code code_usbuser_local

/**
 * The host may put USB peripheral devices in low power suspend mode (by
 * "sending" 3+ms of idle).
 * Once in suspend mode, the host may wake the device back up by sending non-
 * idle state signalling.
 * 
 * This call back is invoked when a wakeup from USB suspend is detected.
 */
void USBCBWakeFromSuspend( void );


/**
 * The USB host sends out a SOF packet to full-speed devices every 1 ms. This
 * interrupt may be useful for isochronous pipes. End designers should
 * implement callback routine as necessary.
 */
void USBCB_SOF_Handler( void );


/**
 * The purpose of this callback is mainly for debugging during development.
 * Check UEIR to see which error causes the interrupt.
 */
void USBCBErrorHandler( void );


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
void USBCBCheckOtherReq( void );


/**
 * The USBCBStdSetDscHandler() callback function is
 * called when a SETUP, bRequest: SET_DESCRIPTOR request
 * arrives. Typically SET_DESCRIPTOR requests are
 * not used in most applications, and it is
 * optional to support this type of request.
 */
void USBCBStdSetDscHandler( void );


/**
 * This function is called when the device becomes
 * initialized, which occurs after the host sends a
 * SET_CONFIGURATION (wValue not = 0) request. This 
 * callback function should initialize the endpoints 
 * for the device's usage according to the current 
 * configuration.
 */
void USBCBInitEP( void );


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
void USBCBSendResume( void );


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL FUNCTIONS
#pragma code code_usbuser_local

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


void USBCB_SOF_Handler( void )
{
    /*
     * No need to clear UIRbits.SOFIF to 0 here. Callback caller is already
     * doing that.
     */
}


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


void USBCBCheckOtherReq( void )
{
    USBCheckHIDRequest();
}


void USBCBStdSetDscHandler( void )
{
    // Must claim session ownership if supporting this request
}


void USBCBInitEP( void )
{
    // Enable the HID endpoint
    USBEnableEndpoint( HID_EP, USB_IN_ENABLED | USB_OUT_ENABLED | USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP );
    
    // Re-arm the OUT endpoint for the next packet
    Usb_RxBufferedPacket();
}


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


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL FUNCTIONS
#pragma code code_usbuser_global

void Usb_UserInit( void )
{
	Debug_PrintConst_Initializing();
    Debug_PrintRom_( "USB" );
    Debug_PrintConst_Dots();
    
    // Reset variables
	usb_outHandle = 0;
	usb_inHandle = 0;
	
	// Initialize USB module
    USBDeviceInit();
    
    Debug_PrintConst_Ok();
    Debug_PrintConst_NewLine();
}


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


void Usb_TxBufferedPacket( void )
{
	usb_inHandle = HIDTxPacket( HID_EP, (BYTE *)&usb_txBuffer[0], sizeof(usb_txBuffer) );
}


void Usb_RxBufferedPacket( void )
{
	usb_outHandle = HIDRxPacket( HID_EP, (BYTE *)&usb_rxBuffer[0], sizeof(usb_rxBuffer) );
}
