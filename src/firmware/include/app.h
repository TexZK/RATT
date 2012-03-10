/**
 * System application global declarations.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

#ifndef __APP_H__
#define	__APP_H__


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS

#include "Compiler.h"
#include "GenericTypeDefs.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER	

// Frequencies
#define SYS_FOSC				48000000		/// Oscillator frequency
#define	SYS_FCY					(SYS_FOSC / 4)	/// Instruction clock rate
#define	GetInstructionClock()	SYS_FCY			/// (Legacy definition for Fcy)

#define	self_power 0							/// (Legacy definition)


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata data_app_global


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES
#pragma code code_app_global

/**
 * Initializes the system.
 */
void App_Initialize( void );


/**
 * One microsecond delay, precise timing.
 */
// 
#define OneUsDelayPrecise()	{Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();}	// @ 48 MHz


/**
 * One microsecond delay, coarse.
 */
void OneUsDelay( void );


#pragma code
#endif	/* !__APP_H__ */
