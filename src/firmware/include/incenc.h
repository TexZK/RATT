/**
 * Incremental encoder driver.
 *
 * This file contains the definitions to handle the incremental encoder.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

#ifndef __INCENC_H__
#define	__INCENC_H__


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

// Pins configuration
#define	INCENC_TRIS_A			TRISCbits.TRISC1	/// Signal A tristate
#define	INCENC_TRIS_B			TRISCbits.TRISC0	/// Signal B tristate

#define	INCENC_PIN_A			PORTCbits.RC1		/// Signal A pin
#define	INCENC_PIN_B			PORTCbits.RC0		/// Signal B pin

#define	INCENC_LAT_A			LATCbits.LATC1		/// Signal A latch
#define	INCENC_LAT_B			LATCbits.LATC0		/// Signal B latch

#define	INCENC_ANS_A			ANSELbits.ANS5		/// Signal A analog selection bit
#define	INCENC_ANS_B			ANSELbits.ANS4		/// Signal B analog selection bit
#define	INCENC_ANSBM_A			(1 << 5)			/// Signal A analog selection bit
#define	INCENC_ANSBM_B			(1 << 4)			/// Signal B analog selection bit


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES

/**
 * Initializes the incremental encoder module.
 */
void IncEnc_Initialize( void );


/**
 * Gets the counter delta insce the last call, then clears it.
 *
 * @return
 *		The counter delta since the last call.
 */
signed short IncEnc_GetDelta( void );


/**
 * Enables incremental encoder interrupts.
 */
#define	IncEnc_EnableInterrupts() {		\
	PIE2bits.C1IE = 1;					\
	PIE2bits.C2IE = 1;					\
}


/**
 * Disables incremental encoder interrupts.
 */
#define	IncEnc_DisableInterrupts() {	\
	PIE2bits.C1IE = 0;					\
	PIE2bits.C2IE = 0;					\
}


/**
 * Signal A interrupt callback.
 */
void IncEnc_CallbackA( void );


/**
 * Signal B interrupt callback.
 */
void IncEnc_CallbackB( void );


#endif	/* !__INCENC_H__ */
