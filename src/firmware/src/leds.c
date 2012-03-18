/**
 * LEDs driver.
 *
 * This file contains the definitions to handle the LEDs.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS

#include "Compiler.h"
#include "leds.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL DEFINITIONS
#pragma udata data_leds_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_leds_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata data_leds_global

#pragma udata access data_leds_global_acs


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL PROTOTYPES
#pragma code code_leds_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL FUNCTIONS
#pragma code code_leds_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL FUNCTIONS
#pragma code code_leds_global

void Leds_Initialize( void )
{
	// Setup pins as outputs
	LEDS_TRIS_GREEN = 0;
	LEDS_TRIS_YELLOW = 0;
	LEDS_TRIS_RED = 0;
	
	// Turn all LEDs off
	GREEN_LED = LED_OFF;
	YELLOW_LED = LED_OFF;
	RED_LED = LED_OFF;
}


// EOF
