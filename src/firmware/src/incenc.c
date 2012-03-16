/**
 * Incremental encoder driver.
 *
 * This file contains the definitions to handle the incremental encoder.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

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

signed short	incenc_delta;			/// Delta accumulator

#pragma udata access data_incenc_local_acs

unsigned char	incenc_lastConfig;		/// Last configuration been read


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
	incenc_lastConfig = 0;
	incenc_delta = 0;
	
	// Enable interrupts
	IncEnc_EnableInterrupts();
}


signed short IncEnc_GetDelta( void )
{
	// TODO
}


void IncEnc_CallbackA( void )
{
	// TODO
}


void IncEnc_CallbackB( void )
{
	// TODO
}


// EOF
