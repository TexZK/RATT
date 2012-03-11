/**
 * System application global declarations.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

#ifndef __BOOTLOADER_H__
#define	__BOOTLOADER_H__


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER	

// ISR vector remapping
#define	REMAPPED_RESET_VECTOR_ADDRESS			0x1000
#define	REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
#define	REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018

// Legacy definitions for USB stack
#define	self_power		1
#define	usb_bus_sense	1


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
// GLOBAL PROTOTYPES

/**
 * Blinks the USB status, according to the current USB state.
 */
void BlinkUSBStatus(void);


#endif	/* !__BOOTLOADER_H__ */
