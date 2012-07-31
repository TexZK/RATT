/**
 * User USB functions.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

#ifndef __USB_USER_H__
#define	__USB_USER_H__


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS

#include "usb/usb.h"
#include "usb/usb_function_hid.h"
#include "app.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata usbram2

// USB buffers
#define	USB_TXBUFFER_SIZE	sizeof(APP_HID_TX_REPORT)		///< Device->Host buffer size, in bytes
#define	USB_RXBUFFER_SIZE	sizeof(APP_HID_TX_REPORT)		///< Host->Device buffer size, in bytes

extern unsigned char	usb_txBuffer[ USB_TXBUFFER_SIZE ];	///< Device->Host data chunk
extern unsigned char	usb_rxBuffer[ USB_RXBUFFER_SIZE ];	///< Host->Device data chunk


#pragma udata data_sys_global

// Handles
extern USB_HANDLE		usb_outHandle;		///< Output handle
extern USB_HANDLE		usb_inHandle;		///< Input handle


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES
#pragma code code_usbuser_global

/**
 * Initializes user's USB features.
 * NOTE: To be called BEFORE the USB initialization!
 */
void Usb_UserInit( void );


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
BOOL USER_USB_CALLBACK_EVENT_HANDLER( USB_EVENT event, void * pdata, WORD size );


/**
 * Sends the buffered packet.
 */
void Usb_TxBufferedPacket( void );


/**
 * Receives the packet into the buffer.
 */
void Usb_RxBufferedPacket( void );


/**
 * Checks whether the USB transmitter is ready or not.
 *
 * @return
 *		Busy if zero, ready otherwise.
 */
#define	Usb_TxReady()	(!HIDTxHandleBusy( usb_inHandle ))


/**
 * Checks whether the USB receivser is ready or not.
 *
 * @return
 *		Busy if zero, ready otherwise.
 */
#define	Usb_RxReady()	(!HIDRxHandleBusy( usb_outHandle ))


#pragma code
#endif	/* !__USB_USER_H__ */
