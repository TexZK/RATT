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
	CM1CON0bits.C1ON = 0;				// Disable Comparator 1
	CM2CON0bits.C2ON = 0;				// Disable Comparator 2
	
	// Setup pins
	INCENC_ANS_A = 1;					// Set as analog pins
	INCENC_ANS_B = 1;
	INCENC_TRIS_A = 1;					// Set as inputs
	INCENC_TRIS_B = 1;
	
	// TODO: Setup Comparator 1
	
	// TODO: Setup Comparator 2
	
	// TODO: Enable interrupts
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
