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
#define	INCENC_TRIS_B			TRISCbits.TRISC2	/// Signal B tristate

#define	INCENC_PIN_A			PORTCbits.RC1		/// Signal A pin
#define	INCENC_PIN_B			PORTCbits.RC2		/// Signal B pin

#define	INCENC_LAT_A			LATCbits.LATC1		/// Signal A latch
#define	INCENC_LAT_B			LATCbits.LATC2		/// Signal B latch

#define	INCENC_ANS_A			ANSELbits.ANS5		/// Signal A analog selection bit
#define	INCENC_ANS_B			ANSELbits.ANS6		/// Signal B analog selection bit
#define	INCENC_ANSBM_A			(1 << 5)			/// Signal A analog selection bit mask
#define	INCENC_ANSBM_B			(1 << 6)			/// Signal B analog selection bit mask


// Interrupt settings
#define	INCENC_INT_IE_A			PIE2bits.C1IE		/// Signal A interrupt switch
#define	INCENC_INT_IE_B			PIE2bits.C2IE		/// Signal B interrupt switch

#define	INCENC_INT_IF_A			PIR2bits.C1IF		/// Signal A interrupt switch
#define	INCENC_INT_IF_B			PIR2bits.C2IF		/// Signal B interrupt switch

#define	INCENC_INT_IP_A			IPR2bits.C1IP		/// Signal A interrupt priority
#define	INCENC_INT_IP_B			IPR2bits.C2IP		/// Signal B interrupt priority
#define	INCENC_INT_IP_VALUE		1					/// Interrupts priority (0 = low, 1 = high)


// Types
typedef unsigned long			INCENC_DELTA;		/// Incremental encoder delta

typedef union {
	struct {
		unsigned	oldState	: 2;				/// Old state
		unsigned	curState	: 2;				/// Current state
		unsigned				: 2;
		unsigned	direction	: 1;				/// Direction (1 = forward, 0 = backward)
		unsigned	dataReady	: 1;				/// Data ready to be processed
	} bits;
	unsigned char	value;							/// Integral value
} INCENC_STATUS;									/// Incremental encoder module status


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES

extern near volatile INCENC_STATUS	incenc_status;	/// Incremental encoder module status


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES

/**
 * Initializes the incremental encoder module.
 */
void IncEnc_Initialize( void );


/**
 * Service which handles all the device management and communication.
 */
void IncEnc_Service( void );


/**
 * Gets the counter delta insce the last call, then clears it.
 *
 * @return
 *		The counter delta since the last call.
 */
INCENC_DELTA IncEnc_GetDelta( void );


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
 * Input changed interrupt callback.
 */
void IncEnc_ChangeCallback( void );


#endif	/* !__INCENC_H__ */
