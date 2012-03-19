/**
 * Incremental encoder driver.
 *
 * This file contains the definitions to handle the incremental encoder.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CONFIGURATION

//#ifndef	__DEBUG
#define	DONT_USE_DEBUG_CONSOLE
//#endif


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS

#include "Compiler.h"
#include "debug.h"
#include "incenc.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL DEFINITIONS
#pragma udata data_incenc_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_incenc_local

volatile signed short	incenc_delta;		/// Delta accumulator

#pragma udata access data_incenc_local_acs

near unsigned char		incenc_state;		/// Incremental state ([3:2] = current inputs, [1:0] = last inputs)


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata data_incenc_global

#pragma udata access data_incenc_global_acs


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL PROTOTYPES
#pragma code code_incenc_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL FUNCTIONS
#pragma code code_incenc_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL FUNCTIONS
#pragma code code_incenc_global

void IncEnc_Initialize( void )
{
	// Disable peripherals
	IncEnc_DisableInterrupts();			// Disable interrupts
	PIR2bits.C1IF = 0;
	PIR2bits.C2IF = 0;
	CM1CON0 = 0;						// Reset comparators
	CM2CON0 = 0;
	CM2CON1 = 0;
	
	// Setup pins
	INCENC_ANS_A = 1;					// Set as analog pins
	INCENC_ANS_B = 1;
	INCENC_TRIS_A = 1;					// Set as inputs
	INCENC_TRIS_B = 1;
	
	// Setup Vdd/2 voltage reference through internal DAC
	REFCON1 = 0;						// Vdd-Vss range, internal
	REFCON2 = 16;						// 50% amplitude
	REFCON1bits.D1EN = 1;				// Enable DAC
	
	// Setup Comparator 1
	CM2CON1bits.C1RSEL = 0;				// C1vref = Vref
	CM1CON0bits.C1R = 1;				// + = C1vref
	CM1CON0bits.C1CH0 = 1;				// - = C12IN1-
	CM1CON0bits.C1CH1 = 0;
	CM1CON0bits.C1SP = 1;				// High-speed
	CM1CON0bits.C1POL = 1;				// Inverted output (= input pin)
	CM1CON0bits.C1ON = 1;				// Enable Comparator 1
	
	// Setup Comparator 2
	CM2CON1bits.C2RSEL = 0;				// C2vref = Vref
	CM2CON0bits.C2R = 1;				// + = C2vref
	CM2CON0bits.C2CH0 = 0;				// - = C12IN2-
	CM2CON0bits.C2CH1 = 1;
	CM2CON0bits.C2SP = 1;				// High-speed
	CM2CON0bits.C2POL = 1;				// Inverted output (= input pin)
	CM2CON0bits.C2ON = 1;				// Enable Comparator 2
	
	// Reset variables
	incenc_state = 0;
	incenc_delta = 0;
	
	// Enable interrupts
	IncEnc_EnableInterrupts();
}


void IncEnc_Service( void )
{
	static signed short last = 0;
	IncEnc_DisableInterrupts();
	if ( incenc_delta != last ) {
		signed short delta = incenc_delta - last;
		last = incenc_delta;
		IncEnc_EnableInterrupts();
		
		// Print a debug event
		Debug_PrintConst_EventBegin();
		Debug_PrintChar( 'e' );
		Debug_PrintS16( delta );
		Debug_PrintConst_EventEnd();
	} else {
		IncEnc_EnableInterrupts();
	}
}


signed short IncEnc_GetDelta( void )
{
	signed short value;
	IncEnc_DisableInterrupts();
	value = incenc_delta;
	incenc_delta = 0;
	IncEnc_EnableInterrupts();
	return value;
}


void IncEnc_ChangeCallback( void )
{
	// Sample the current state
	incenc_state = (incenc_state >> 2) & 0b0011;	// Old state in bits 1:0
	incenc_state |= (CM2CON1 >> 4) & 0b1100;		// Current state in bits 3:2
	
	/* (A;B)
	 * Clockwise pattern:
	 *  00 -> 01 -> 11 -> 10 -> 00
	 * Counter-clockwise pattern:
	 *  00 -> 10 -> 11 -> 01 -> 00
	 */
	switch ( incenc_state ) {
		case 0b0001:
		case 0b0111:
		case 0b1110:
		case 0b1000: {
			++incenc_delta;
			break;
		}
		case 0b0010:
		case 0b1011:
		case 0b1101:
		case 0b0100: {
			--incenc_delta;
			break;
		}
	}
}


// EOF
