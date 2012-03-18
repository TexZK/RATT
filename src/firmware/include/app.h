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


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

// Frequencies
#define SYS_FOSC				48000000			/// Oscillator frequency
#define	SYS_FCY					(SYS_FOSC / 4)		/// Instruction clock rate
#define	GetInstructionClock()	SYS_FCY				/// (Legacy definition for Fcy)


// Bootloader/User button
#define	BUTTON_TRIS				TRISAbits.TRISA3	/// User button tristate (always = 1)
#define	BUTTON_PIN				PORTAbits.RA3		/// User button pin, active low

// Auxiliary input pin
#define	AUXIN_TRIS				TRISCbits.TRISC6	/// Aux input tristate
#define	AUXIN_PIN				PORTCbits.RC6		/// Aux input pin
#define	AUXIN_LAT				LATCbits.LATC6		/// Aux input latch
#define	AUXIN_ANS				ANSELHbits.ANS8		/// Aux input analog selection bit
#define	AUXIN_ANS_VALUE			1					/// Aux input analog selection value (1 = analog, 0 = digital)
#define	AUXIN_ANSHBM			0b00000001			/// Aux input analog selection bitmask


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES

/**
 * Initializes the system.
 */
void App_Initialize( void );


/**
 * Wait for button press, after being released.
 */
void WaitButtonPress( void );


/**
 * One microsecond delay, precise timing.
 */
#define	OneUsDelayPrecise()	{Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();}	// @ 48 MHz


/**
 * One microsecond delay, with possible compiler-dependent jitter.
 */
void OneUsDelay( void );


#endif	/* !__APP_H__ */
