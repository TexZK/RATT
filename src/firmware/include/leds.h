/**
 * LEDs driver.
 *
 * This file contains the definitions to handle the LEDs.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

#ifndef __LEDS_H__
#define	__LEDS_H__


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

// Pins configuration
#define	LEDS_PORT				PORTC				/// LEDs port
#define	LEDS_TRIS				TRISC				/// LEDs port tristate
#define	LEDS_MASK				0b00111000			/// LEDs port/tristate mask

#define	LEDS_TRIS_GREEN			TRISCbits.TRISC3	/// Green LED pin tristate
#define	LEDS_TRIS_YELLOW		TRISCbits.TRISC4	/// Yellow LED pin tristate
#define	LEDS_TRIS_RED			TRISCbits.TRISC5	/// Red LED pin tristate

#define	LEDS_PIN_GREEN			PORTCbits.RC3		/// Green LED pin
#define	LEDS_PIN_YELLOW			PORTCbits.RC4		/// Yellow LED pin
#define	LEDS_PIN_RED			PORTCbits.RC5		/// Red LED pin

#define	LEDS_LAT_GREEN			LATCbits.LATC3		/// Green LED latch

#define	LEDS_LAT_YELLOW			LATCbits.LATC4		/// Yellow LED latch
#define	LEDS_LAT_RED			LATCbits.LATC5		/// Red LED latch


// Shortcuts
#define	GREEN_LED				LEDS_LAT_GREEN
#define	YELLOW_LED				LEDS_LAT_YELLOW
#define	RED_LED					LEDS_LAT_RED

#define	LED_ON					0					/// Incoming current
#define	LED_OFF					1					/// Pull-up


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES

/**
 * Initializes the LEDs module.
 */
void Leds_Initialize( void );


/**
 * Turns LEDs as if they were bits:
 * 	GREEN	= bit 0,
 *	YELLOW	= bit 1,
 *	RED		= bit 2
 *
 * @param value
 *		3-bits value [RYG].
 */
#define	Leds_BinaryValue( value )	{	\
	GREEN_LED = !((value) & 0b001);		\
	YELLOW_LED = !((value) & 0b010);	\
	RED_LED = !((value) & 0b100);		\
}


#endif	/* !__LEDS_H__ */
