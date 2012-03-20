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
#include "app.h"
#include "leds.h"
#include "debug.h"
#include "incenc.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL DEFINITIONS
#pragma udata data_incenc_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_incenc_local

volatile INCENC_DELTA		incenc_delta;		/// Delta accumulator


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata access data_incenc_global_acs

near volatile INCENC_STATUS	incenc_status;


#pragma udata data_incenc_global


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
	INCENC_INT_IF_A = 0;
	INCENC_INT_IF_B = 0;
	CM1CON0 = 0;						// Reset comparators
	CM2CON0 = 0;
	CM2CON1 = 0;
	CCP1CON = 0;						// Do not use Capture/Compare/PWM!
	
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
	CM2CON1bits.C1HYS = 1;				// Enable hysteresis
	CM1CON0bits.C1ON = 1;				// Enable Comparator 1
	
	// Setup Comparator 2
	CM2CON1bits.C2RSEL = 0;				// C2vref = Vref
	CM2CON0bits.C2R = 1;				// + = C2vref
	CM2CON0bits.C2CH0 = 0;				// - = C12IN2-
	CM2CON0bits.C2CH1 = 1;
	CM2CON0bits.C2SP = 1;				// High-speed
	CM2CON0bits.C2POL = 1;				// Inverted output (= input pin)
	CM2CON1bits.C2HYS = 1;				// Enable hysteresis
	CM2CON0bits.C2ON = 1;				// Enable Comparator 2
	
	// Reset variables
	incenc_status.value = 0;
	incenc_delta = 0;
	
	// Enable interrupts
	INCENC_INT_IP_A = INCENC_INT_IP_VALUE;
	INCENC_INT_IP_B = INCENC_INT_IP_VALUE;
	OneUsDelay();
	OneUsDelay();
	IncEnc_EnableInterrupts();
}


void IncEnc_Service( void )
{
	// Nothing to do
}


INCENC_DELTA IncEnc_GetDelta( void )
{
	INCENC_DELTA delta;
	App_Lock();
	delta = incenc_delta;
	incenc_delta = 0;
	App_Unlock();
	return delta;
}


void IncEnc_ChangeCallback( void )
{
	static volatile unsigned char state, dummy;
	
	// Sample the current state
	state = (incenc_status.value >> 2) & 0b0011;	// Old state in bits 1:0
	state |= (CM2CON1 >> 4) & 0b1100;				// Current state in bits 3:2
	
	// Clear the mismatch condition and interrupt flags
	dummy = CM1CON0;
	dummy = CM2CON0;
	INCENC_INT_IF_A = 0;
	INCENC_INT_IF_B = 0;
	
	/* [AB]:
	 * Clockwise pattern:
	 *  [00] -> [01] -> [11] -> [10] -> [00]
	 * Counter-clockwise pattern:
	 *  [00] -> [10] -> [11] -> [01] -> [00]
	 * We do not care about direction changes
	 *
	 * 0bNNLL: LL = old, NN = new
	 */
	switch ( state ) {
		case 0b0100:
		case 0b1101:
		case 0b1011:
		case 0b0010: {
			// Going forward
			++incenc_delta;
			incenc_status.bits.direction = 1;
			break;
		}
		case 0b1000:
		case 0b1110:
		case 0b0111:
		case 0b0001: {
			// Going backwards
			--incenc_delta;
			incenc_status.bits.direction = 0;
			break;
		}
		case 0b1100:
		case 0b0110:
		case 0b0011:
		case 0b1001: {
			// Invalid states
		}
	}
	
	incenc_status.value &= 0xF0;
	incenc_status.value |= state;
	incenc_status.bits.dataReady = 1;
}


// EOF
