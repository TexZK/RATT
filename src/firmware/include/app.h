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

#include "Adns2080.h"
#include "IncEnc.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

// Frequencies
#define SYS_FOSC				48000000			///< Oscillator frequency
#define	SYS_FCY					(SYS_FOSC / 4)		///< Instruction clock rate
#define	GetInstructionClock()	SYS_FCY				///< (Legacy definition for Fcy)


// Bootloader/User button
#define	BUTTON_TRIS				TRISAbits.TRISA3	///< User button tristate (always = 1)
#define	BUTTON_PIN				PORTAbits.RA3		///< User button pin, active low

// Auxiliary input pin
#define	AUXIN_TRIS				TRISCbits.TRISC6	///< Aux input tristate
#define	AUXIN_PIN				PORTCbits.RC6		///< Aux input pin
#define	AUXIN_LAT				LATCbits.LATC6		///< Aux input latch
#define	AUXIN_ANS				ANSELHbits.ANS8		///< Aux input analog selection bit
#define	AUXIN_ANS_VALUE			1					///< Aux input analog selection value (1 = analog, 0 = digital)
#define	AUXIN_ANSHBM			0b00000001			///< Aux input analog selection bitmask


// Structures

typedef	union {
	struct {
		unsigned	inLowIrq	: 1;				///< Serving a low interrupt request
		unsigned	inHighIrq	: 1;				///< Serving a high interrupt request
		unsigned				: 6;
	} bits;
	unsigned char value;
} APP_STATUS;										///< Application status


typedef struct {
	unsigned long			id;						///< Report ID
	unsigned long			timestamp;				///< Report timestamp
	ADNS_DELTAS				mouseMotion;			///< Motion from mouse sensor
	INCENC_DELTA			incencMotion;			///< Motion from incremental encoder
} APP_HID_TX_REPORT;								///< HID TX report


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES

extern volatile unsigned long	app_timestamp;		///< Application timestamp
extern near volatile APP_STATUS	app_status;			///< Application status


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES

/**
 * Initializes the system.
 */
void App_Initialize( void );


/**
 * Enables interrupts, globally.
 */
#define App_EnableGlobalInterrupts() {	\
	INTCONbits.GIEH = 1;				\
	INTCONbits.GIEL = 1;				\
}


/**
 * Disables interrupts, globally.
 */
#define App_DisableGlobalInterrupts() {	\
	INTCONbits.GIEL = 0;				\
	INTCONbits.GIEH = 0;				\
}


/**
 * Enables high priority interrupts.
 */
#define App_EnableHighInterrupts() {	\
	INTCONbits.GIEH = 1;				\
}


/**
 * Disables high priority interrupts.
 */
#define App_DisableHighInterrupts() {	\
	INTCONbits.GIEH = 0;				\
}


/**
 * Enables low priority interrupts.
 */
#define App_EnableLowInterrupts() {		\
	INTCONbits.GIEL = 1;				\
}


/**
 * Disables low priority interrupts.
 */
#define App_DisableLowInterrupts() {	\
	INTCONbits.GIEL = 0;				\
}


/**
 * Shorthand for App_DisableGlobalInterrupts().
 */
#define	App_Lock()			App_DisableGlobalInterrupts()


/**
 * Shorthand for App_EnableGlobalInterrupts().
 */
#define	App_Unlock()		App_EnableGlobalInterrupts()


/**
 * Wait for button press, after being released.
 */
void App_WaitButtonPress( void );


/**
 * Gets the current timestamp.
 *
 * @return
 *		The current timestamp.
 */
unsigned long App_GetTimestamp( void );

/**
 * One microsecond delay, precise timing.
 */
#define	OneUsDelayPrecise()	{Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();}	// @ 48 MHz


/**
 * One microsecond delay, with possible compiler-dependent jitter.
 */
void OneUsDelay( void );


#endif	/* !__APP_H__ */
