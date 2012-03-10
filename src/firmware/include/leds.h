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


// Shortcuts
#define	GREEN_LED				LEDS_PIN_GREEN
#define	YELLOW_LED				LEDS_PIN_YELLOW
#define	RED_LED					LEDS_PIN_RED

#define	LED_ON					0					/// Incoming current
#define	LED_OFF					1					/// Pull-up


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata data_leds_global


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES
#pragma code code_leds_global

/**
 * Initializes the LEDs module.
 */
void Leds_Initialize( void );


#pragma code
#endif	/* !__LEDS_H__ */
