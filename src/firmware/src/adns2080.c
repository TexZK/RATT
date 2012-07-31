/**
 * Avago ADNS-2080 driver.
 *
 * This file contains the function calls in order to communicate to an
 * Avago ADNS-2080 device through the SPI port of the PIC18F14K50.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CONFIGURATION

#define	DONT_USE_DEBUG_CONSOLE


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS

#include "Compiler.h"
#include "app.h"
#include "leds.h"
#include "debug.h"
#include "TimeDelay.h"
#include "usb/usb_user.h"
#include "adns2080.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL DEFINITIONS
#pragma udata data_adns_local

/**
 * The proprtype device is affected by bug 2.1 described in "PIC18(L)F1XK50
 * Silicon Errata and Data Sheet Clarification" (document #DS80411E).
 */
#define	USE_WORKAROUND_DS80411E_2_1_OPT2

#define	PR2_VALUE	(SYS_FCY / (ADNS_SPI_FREQ * 2) - 1)		///< PR2 value for Timer 2 time base


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_adns_local

volatile ADNS_DELTAS	adns_deltas;				///< Deltas accumulator


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata access data_adns_global_acs

near ADNS_STATUS		adns_status;				///< Device status


#pragma udata data_adns_global


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL PROTOTYPES
#pragma code code_adns_local

/**
 * Sends a byte over SPI module.
 *
 * @param value
 *		Value to be sent.
 */
void Adns_WriteSPI( unsigned char value );


/**
 * Receives a byte from the SPI module.
 *
 * @return
 *		Value been received.
 */
unsigned char Adns_ReadSPI( void );


/**
 * Configures ADNS registers.
 */
void Adns_SetupConfiguration( void );


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL FUNCTIONS
#pragma code code_adns_local

void Adns_WriteSPI( unsigned char value )
{
	volatile unsigned char dummy;
	dummy = SSPBUF;						// Clear buffer state
	SSPCON1bits.WCOL = 0;
	PIR1bits.SSPIF = 0;					// Clear the interrupt flag
	
#ifdef USE_WORKAROUND_DS80411E_2_1_OPT2
	T2CONbits.TMR2ON = 0;				// Disable the Timer 2 time base
	TMR2 = 0;							// Clear the Timer 2 counter
#endif
	ADNS_TRIS_MOSI = 0;					// Set MOSI as output
	SSPBUF = value;						// Write the value to be sent
#ifdef USE_WORKAROUND_DS80411E_2_1_OPT2
	T2CONbits.TMR2ON = 1;				// Enable the Timer 2 time base again
#endif
	if ( !SSPCON1bits.WCOL ) {			// Check for no collision
		while ( !PIR1bits.SSPIF );		// Wait until the value is sent
	}
}


unsigned char Adns_ReadSPI( void )
{
	volatile unsigned char dummy;
	ADNS_TRIS_MOSI = 1;					// Set MOSI as input (should already be HiZ, this is only to be safe!)
	dummy = SSPBUF;						// Clear buffer state
	SSPCON1bits.WCOL = 0;
	PIR1bits.SSPIF = 0;					// Clear the interrupt flag
	
#ifdef USE_WORKAROUND_DS80411E_2_1_OPT2
	T2CONbits.TMR2ON = 0;				// Disable the Timer 2 time base
	TMR2 = 0;							// Clear the Timer 2 counter
#endif
	SSPBUF = 0xFF;						// Keep MOSI high
#ifdef USE_WORKAROUND_DS80411E_2_1_OPT2
	T2CONbits.TMR2ON = 1;				// Enable the Timer 2 time base again
#endif
	while ( !PIR1bits.SSPIF );			// Wait until the value is sent
	
	return SSPBUF;
}


void Adns_SetupConfiguration( void )
{
	ADNS_BITS_MOUSE_CTRL	mouse_ctrl;
	ADNS_BITS_MOTION_CTRL	motion_ctrl;
	ADNS_BITS_PERFORMANCE	performance;
	unsigned char			value, expected;
	
	Debug_PrintConst_Initializing();
	Debug_PrintRom_( "ADNS registers" );
	Debug_PrintConst_Dots();
	Debug_PrintConst_NewLine();
	
	// Setup Mouse Control
	Debug_PrintRom_( "\tMOUSE_CTRL" );
	
	mouse_ctrl.value = ADNS_DEF_MOUSE_CTRL;
	mouse_ctrl.bits.BIT_REPORTING = 1;		// 12-bits reporting
	Adns_WriteBlocking( ADNS_REG_MOUSE_CTRL, mouse_ctrl.value );
	Adns_WriteReadDelay();
	value = Adns_ReadBlocking( ADNS_REG_MOUSE_CTRL );
	
	Debug_PrintConst_Eq();
	Debug_PrintConst_0x();
	Debug_PrintHex( value );
	expected = mouse_ctrl.value;
	if ( value != expected ) {
		goto failure;
	}
	Debug_PrintConst_NewLine();

	// Setup Motion Control
	Debug_PrintRom_( "\tMOTION_CTRL" );
	
	motion_ctrl.value = ADNS_DEF_MOTION_CTRL;
	motion_ctrl.bits.MOT_A = 0;				// Active low
	motion_ctrl.bits.MOT_S = 0;				// Level sensitive (not needed, but might be useful for debugging)
	Adns_WriteBlocking( ADNS_REG_MOTION_CTRL, motion_ctrl.value );
	Adns_WriteReadDelay();
	value = Adns_ReadBlocking( ADNS_REG_MOTION_CTRL );
	
	Debug_PrintConst_Eq();
	Debug_PrintConst_0x();
	Debug_PrintHex( value );
	expected = motion_ctrl.value;
	if ( value != expected ) {
		goto failure;
	}
	Debug_PrintConst_NewLine();
	
	// Set Motion Burst index boundaries
	Debug_PrintRom_( "\tBURST_READ_FIRST" );
	
	Adns_WriteBlocking( ADNS_REG_BURST_READ_FIRST, ADNS_REG_DELTA_X );
	Adns_WriteReadDelay();
	value = Adns_ReadBlocking( ADNS_REG_BURST_READ_FIRST );
	
	Debug_PrintConst_Eq();
	Debug_PrintConst_0x();
	Debug_PrintHex( value );
	expected = ADNS_REG_DELTA_X;
	if ( value != expected ) {
		goto failure;
	}
	Debug_PrintConst_NewLine();
	
	Debug_PrintRom_( "\tBURST_READ_LAST" );
	
	Adns_WriteBlocking( ADNS_REG_BURST_READ_LAST, ADNS_REG_DELTA_XY_HIGH );
	Adns_WriteReadDelay();
	value = Adns_ReadBlocking( ADNS_REG_BURST_READ_LAST );
	
	Debug_PrintConst_Eq();
	Debug_PrintConst_0x();
	Debug_PrintHex( value );
	expected = ADNS_REG_DELTA_XY_HIGH;
	if ( value != expected ) {
		goto failure;
	}
	Debug_PrintConst_NewLine();
	
	// Everything configured correctly
	Debug_PrintConst_Dots();
	Debug_PrintConst_Ok();
	Debug_PrintConst_NewLine();
	return;
	
failure:
	// Some values are wrong
	Debug_PrintRom_( " != " );
	Debug_PrintConst_0x();
	Debug_PrintHex( expected );
	Debug_PrintConst_NewLine();
	Debug_PrintConst_Dots();
	Debug_PrintConst_Fail();
	Debug_PrintConst_NewLine();
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL FUNCTIONS
#pragma code code_adns_global

void Adns_Initialize( void )
{
	// Disable SPI and interrupts
	Adns_DisableInterrupt();		// Disable MOTION interrupt
	PIE1bits.SSPIE = 0;				// Clear SPI interrupt
	PIR1bits.SSPIF = 0;
	IPR1bits.SSPIP = 0;
	
	SSPCON1bits.SSPEN = 0;			// Reset the SPI configuration
	SSPSTAT = 0;
	SSPCON1 = 0;
	SSPCON2 = 0;
	
	// Setup pins
	ADNS_LAT_MOSI = 1;
	ADNS_LAT_SCLK = 1;
	
	ADNS_TRIS_MISO = 1;
	ADNS_TRIS_MOSI = 0;
	ADNS_TRIS_SCLK = 0;
	ADNS_TRIS_MOTION = 1;
	
	// Print debug info
	Debug_PrintConst_Initializing();
	Debug_PrintRom_( "SPI" );
	Debug_PrintConst_Dots();
	
	// Setup Timer 2 time base
	PIE1bits.TMR2IE = 0;			// Disable Timer 2 interrupts
	PIR1bits.TMR2IF = 0;
	T2CON = 0;						// No prescaler, no postscaler, timer disabled
	PR2 = PR2_VALUE;				// Desired time base at TMR2/2
	T2CONbits.TMR2ON = 1;			// Enable the Timer 2 time base
	
	// Configure the SPI module
	SSPSTAT = 0;					// Reset the MSSP status register
	SSPSTATbits.SMP = 0;			// Data valid at half clock period
	SSPCON1bits.CKP = 1;			// Idle clock state at high level
	SSPSTATbits.CKE = 0;			// Data transition at high->low clock edge
	SSPCON1bits.SSPM0 = 1;			// TMR2/2 as time base (SSPM = 0b0011)
	SSPCON1bits.SSPM1 = 1;
	SSPCON1bits.SSPEN = 1;			// Enable the SPI module

	// Reset variables
	adns_status.motionInt = 0;
	adns_status.dataReady = 0;
	
	adns_deltas.dx = 0L;
	adns_deltas.dy = 0L;
	
	Adns_PowerUpDelay();
	Debug_PrintConst_Ok();
	Debug_PrintConst_NewLine();
	
	// Reset the device
	Adns_WriteBlocking( ADNS_REG_RESET, ADNS_CMD_RESET );
	Adns_ResetCommunication();
	
	// Check communication
	if ( Adns_CheckCommunication() ) {
		// Configure the expected behavior
		Adns_SetupConfiguration();
		
		// Clear any previous motion
		DelayMs( ADNS_DLY_MOT_RST_MAX_MS );		// Wait for valid motion detection
		Adns_WriteReadDelay();
		Adns_BurstReadMotionDeltasBlocking();
		
		// Enable MOTION interrupt
		ADNS_INT_IF = 0;
		ADNS_INT_EDGE = ADNS_INT_EDGE_VALUE;
		Adns_EnableInterrupt();
	}
}


void Adns_PowerUpDelay( void )
{
	DelayMs( ADNS_DLY_MOT_RST_MAX_MS );
}


void Adns_Service( void )
{
#if !ADNS_USE_INTERRUPT
	// Poll for motion status
	if ( ADNS_PIN_MOTION == ADNS_INT_EDGE_VALUE ) {
		adns_status.motionInt = 1;
		RED_LED = LED_ON;
	}
#endif
	
	// Process motion if detected
	if ( adns_status.motionInt ) {
		Adns_DisableInterrupt();
		adns_status.motionInt = 0;
		Adns_EnableInterrupt();
		
		Adns_BurstReadMotionDeltasBlocking();
		
		adns_status.dataReady = 1;
		RED_LED = LED_OFF;
		YELLOW_LED = LED_ON;
	}
}


void Adns_MotionCallback( void )
{
	ADNS_INT_IF = 0;
	adns_status.motionInt = 1;
	RED_LED = LED_ON;
}	


unsigned char Adns_CheckCommunication( void )
{
	unsigned char value, inverted;
	
	Debug_PrintConst_Checking();
	Debug_PrintRom_( "ADNS connection" );
	Debug_PrintConst_Dots();
	Debug_PrintConst_NewLine();
	
	Debug_PrintChar( '\t' );
	Debug_PrintConst_Checking();
	Debug_PrintRom_( "PROD_ID" );
	Debug_PrintConst_Eq();
	value = Adns_ReadBlocking( ADNS_REG_PROD_ID );
	Adns_ReadSubsequentDelay();
	Debug_PrintConst_0x();
	Debug_PrintHex( value );
	Debug_PrintConst_NewLine();
	
	if ( value != ADNS_DEF_PROD_ID ) {
		Debug_PrintConst_Dots();
		Debug_PrintConst_Fail();
		Debug_PrintConst_NewLine();
		return 0;
	}
	
	Debug_PrintChar( '\t' );
	Debug_PrintConst_Checking();
	Debug_PrintRom_( "REV_ID" );
	Debug_PrintConst_Eq();
	value = Adns_ReadBlocking( ADNS_REG_REV_ID );
	Adns_ReadSubsequentDelay();
	Debug_PrintConst_0x();
	Debug_PrintHex( value );
	Debug_PrintConst_NewLine();
	
	if ( value != ADNS_DEF_REV_ID ) {
		Debug_PrintConst_Dots();
		Debug_PrintConst_Fail();
		Debug_PrintConst_NewLine();
		return 0;
	}
	
	Debug_PrintChar( '\t' );
	Debug_PrintConst_Checking();
	Debug_PrintRom_( "NOT_REV_ID" );
	Debug_PrintConst_Eq();
	inverted = Adns_ReadBlocking( ADNS_REG_NOT_REV_ID );
	Adns_ReadSubsequentDelay();
	Debug_PrintConst_0x();
	Debug_PrintHex( inverted );
	Debug_PrintConst_NewLine();
	
	if ( inverted != ADNS_DEF_NOT_REV_ID ) {
		Debug_PrintConst_Dots();
		Debug_PrintConst_Fail();
		Debug_PrintConst_NewLine();
		return 0;
	}
	
	if ( inverted != ~value ) {
		Debug_PrintConst_Dots();
		Debug_PrintConst_Fail();
		Debug_PrintConst_NewLine();
		return 0;
	}
	
	Debug_PrintConst_Dots();
	Debug_PrintConst_Ok();
	Debug_PrintConst_NewLine();
	return 1;
}


void Adns_ResetCommunication( void )
{
	DelayMs( ADNS_DLY_TIMEOUT_SDIO_MIN_MS + 1 );
}


void Adns_WriteBlocking( unsigned char address, unsigned char value )
{
	Adns_WriteSPI( address | ADNS_WRITE_OR_MASK );	// Write the register address
	Adns_WriteSPI( value );			// Write the register value
}


unsigned char Adns_ReadBlocking( unsigned char address )
{
	Adns_WriteSPI( address | ADNS_READ_OR_MASK );	// Write the register address
	Adns_ReadSubsequentDelay();		// Used as t_HOLD-SDIO
	
	ADNS_TRIS_MOSI = 1;				// Set MOSI to HiZ
	Adns_AddressDataDelay();
	address = Adns_ReadSPI();		// Read the register value
	
	Adns_ReadSubsequentDelay();		// Used as t_HOLD-READ
	return address;
}


void Adns_BurstReadMotionDeltasBlocking( void )
{
	static union {
		struct {
			unsigned char	lo;
			unsigned char	hi;
		} bytes;
		signed long			value;
	} split;
	static unsigned char			dx, dy, dxyh;
	
	// Call a Motion Burst message chain
	Adns_WriteSPI( ADNS_REG_MOTION_BURST | ADNS_READ_OR_MASK );
	Adns_ReadSubsequentDelay();				// Used as t_HOLD-SDIO
	ADNS_TRIS_MOSI = 1;						// Set MOSI to HiZ
	Adns_AddressDataDelay();
	
	// Read the chosen Motion Burst registers
	dx = Adns_ReadSPI();					// DELTA_X
	dy = Adns_ReadSPI();					// DELTA_Y
	dxyh = Adns_ReadSPI();					// DELTA_XY_HIGH
	
	// Compute Delta X as a 16-bits signed integer
	split.bytes.lo = dx;
	split.bytes.hi = (dxyh >> 4) & 0x0F;
	if ( split.bytes.hi & 0x08 ) {
		split.bytes.hi |= 0xF0;				// Sign extension
	}
	adns_deltas.dx += split.value;			// Accumulate
	
	// Compute Delta Y as a 16-bits signed integer
	split.bytes.lo = dy;
	split.bytes.hi = dxyh & 0x0F;
	if ( split.bytes.hi & 0x08 ) {
		split.bytes.hi |= 0xF0;				// Sign extension
	}
	adns_deltas.dy += split.value;			// Accumulate
}	


void Adns_WriteWriteDelay( void )
{
	Delay10us( (ADNS_DLY_TSWW_MIN_MS + 9 - 2 * ADNS_DLY_SPI_BYTE_US) / 10 );
}


void Adns_WriteReadDelay( void )
{
	Delay10us( (ADNS_DLY_TSWR_MIN_MS + 9 - ADNS_DLY_SPI_BYTE_US) / 10 );
}


void Adns_AddressDataDelay( void )
{
	OneUsDelay();
	OneUsDelay();
	OneUsDelay();
	OneUsDelay();
}


void Adns_ReadSubsequentDelay( void )
{
	Nop();		// Actually, this delay is already
	Nop();		// spent by overhead instructions
	Nop();
	Nop();
}


void Adns_ClearDeltas( void )
{
	Adns_DisableInterrupt();
	adns_deltas.dx = 0;
	adns_deltas.dy = 0;
	adns_status.dataReady = 0;
	Adns_EnableInterrupt();
}

// EOF
