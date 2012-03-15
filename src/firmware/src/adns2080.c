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

#define	PR2_VALUE	(SYS_FCY / (ADNS_SPI_FREQ * 2) - 1)		/// PR2 value for Timer 2 time base


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_adns_local

ADNS_HID_DATA	adns_hidData;			/// HID report data


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata access data_adns_global_access

near ADNS_STATUS	adns_status;		/// Device status


#pragma udata data_adns_global

signed short		adns_deltaY;		/// Copy of Delta_Y register been read
signed short		adns_deltaX;		/// Copy of Delta_X register been read

signed short		adns_y;				/// Y position since last HID report
signed short		adns_x;				/// X position since last HID report


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
	dummy = SSPBUF;					// Clear buffer state
	SSPCON1bits.WCOL = 0;
	PIR1bits.SSPIF = 0;				// Clear the interrupt flag
	
#ifdef USE_WORKAROUND_DS80411E_2_1_OPT2
	T2CONbits.TMR2ON = 0;			// Disable the Timer 2 time base
	TMR2 = 0;						// Clear the Timer 2 counter
#endif
	SSPBUF = value;					// Write the value to be sent
#ifdef USE_WORKAROUND_DS80411E_2_1_OPT2
	T2CONbits.TMR2ON = 1;			// Enable the Timer 2 time base again
#endif
	if ( !SSPCON1bits.WCOL ) {		// Check for no collision
		while ( !PIR1bits.SSPIF );	// Wait until the value is sent
	}
}


unsigned char Adns_ReadSPI( void )
{
	volatile unsigned char dummy;
	dummy = SSPBUF;					// Clear buffer state
	SSPCON1bits.WCOL = 0;
	PIR1bits.SSPIF = 0;				// Clear the interrupt flag
	
#ifdef USE_WORKAROUND_DS80411E_2_1_OPT2
	T2CONbits.TMR2ON = 0;			// Disable the Timer 2 time base
	TMR2 = 0;						// Clear the Timer 2 counter
#endif
	SSPBUF = 0xFF;					// Keep MOSI high
#ifdef USE_WORKAROUND_DS80411E_2_1_OPT2
	T2CONbits.TMR2ON = 1;			// Enable the Timer 2 time base again
#endif
	while ( !PIR1bits.SSPIF );		// Wait until the value is sent
	return SSPBUF;
}


void Adns_SetupConfiguration( void )
{
	ADNS_BITS_MOUSE_CTRL	mouse_ctrl;
	ADNS_BITS_MOTION_CTRL	motion_ctrl;
	ADNS_BITS_PERFORMANCE	performance;
	
	// Setup Mouse Control
	mouse_ctrl.value = ADNS_DEF_MOUSE_CTRL;
	mouse_ctrl.bits.BIT_REPORTING = 1;		// 12-bits reporting
	Adns_WriteBlocking( ADNS_REG_MOUSE_CTRL, mouse_ctrl.value );
	
	// Setup Performance
	performance.value = ADNS_DEF_PERFORMANCE;
	performance.bits.FORCE = 0x04;			// Run 1
	Adns_WriteBlocking( ADNS_REG_PERFORMANCE, performance.value );
	
	// Setup Motion Control
	motion_ctrl.value = ADNS_DEF_MOTION_CTRL;
	motion_ctrl.bits.MOT_A = 0;				// Active low
	motion_ctrl.bits.MOT_S = 0;				// Level sensitive (not needed, but might be useful for debugging)
	Adns_WriteBlocking( ADNS_REG_MOTION_CTRL, motion_ctrl.value );
	
	// Set Motion Burst index boundaries
	Adns_WriteBlocking( ADNS_REG_BURST_READ_FIRST, ADNS_REG_DELTA_X );
	Adns_WriteBlocking( ADNS_REG_BURST_READ_LAST, ADNS_REG_DELTA_XY_HIGH );
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
	Debug_PrintRom_( "ADNS" );
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
	
	adns_deltaX = 0;
	adns_deltaY = 0;
	adns_x = 0;
	adns_y = 0;
	
	adns_hidData.reportID = 0;
	adns_hidData.deltaX = 0;
	adns_hidData.deltaY = 0;
	
	Adns_PowerUpDelay();
	Debug_PrintConst_Ok();
	Debug_PrintConst_NewLine();
	
	// Reset the device
	Debug_PrintConst_Checking();
	Debug_PrintRom_( "ADNS connection" );
	Debug_PrintConst_Dots();
	Adns_WriteBlocking( ADNS_REG_RESET, ADNS_DEF_RESET );
	Adns_ResetCommunication();
	
	// Check communication
	if ( Adns_CheckCommunication() ) {
		Debug_PrintConst_Ok();
		
		// Configure the expected behavior
		Adns_SetupConfiguration();
		
		// Enable MOTION interrupt
		ADNS_INT_IF = 0;
		ADNS_INT_IP = 1;
		ADNS_INT_EDGE = 1;
		DelayMs( ADNS_DLY_MOT_RST_MAX_MS );		// Wait for valid motion detection
		Adns_EnableInterrupt();
	} else {
		Debug_PrintConst_Fail();
	}
}


void Adns_PowerUpDelay( void )
{
	DelayMs( ADNS_DLY_MOT_RST_MAX_MS );
}


void Adns_Service( void )
{
	// Check for motion interrupt
	if ( adns_status.motionInt ) {
		ADNS_BURST_MOTION_DELTAS burst;
		
		Adns_DisableInterrupt();
		adns_status.motionInt = 0;
		Adns_ResetCommunication();
		burst = Adns_BurstReadMotionDeltasBlocking();
		YELLOW_LED = LED_ON;			// Yellow LED for cached HID packet
		RED_LED = LED_OFF;
		Adns_EnableInterrupt();
		
		adns_deltaY = burst.deltaY;
		adns_deltaX = burst.deltaX;
		
		adns_x += (signed short)adns_deltaX;
		adns_y += (signed short)adns_deltaY;
		adns_status.dataReady = 1;
	}
	
	// Check if a HID packet can be sent
	if ( adns_status.dataReady ) {
		if ( Usb_TxReady() ) {
			// Build the packet
			Adns_DisableInterrupt();
			++adns_hidData.reportID;
			adns_hidData.deltaX = adns_x;
			adns_hidData.deltaY = adns_y;
			Adns_EnableInterrupt();
			*((ADNS_HID_DATA *)usb_txBuffer) = adns_hidData;
			
			// Send the HID packet
			Usb_TxBufferedPacket();
			
			// Clear the delta state
			Adns_DisableInterrupt();
			adns_status.dataReady = 0;
			adns_x = 0;
			adns_y = 0;
			Adns_EnableInterrupt();
			
			YELLOW_LED = LED_OFF;
		}
	}	
}


void Adns_MotionCallback( void )
{
	ADNS_INT_IF = 0;
	adns_status.motionInt = 1;
	RED_LED = LED_ON;					// Red LED for cached motion interrupt
}	


unsigned char Adns_CheckCommunication( void )
{
	unsigned char value;
	
	if ( Adns_ReadBlocking( ADNS_REG_PROD_ID ) != ADNS_DEF_PROD_ID ) {
		Adns_ReadSubsequentDelay();
		return 0;
	}
	Adns_ReadSubsequentDelay();
	value = ~Adns_ReadBlocking( ADNS_REG_REV_ID );
	Adns_ReadSubsequentDelay();
	if ( Adns_ReadBlocking( ADNS_REG_NOT_REV_ID ) != value ) {
		Adns_ReadSubsequentDelay();
		return 0;
	}
	Adns_ReadSubsequentDelay();
	return 1;
}


void Adns_ResetCommunication( void )
{
	DelayMs( ADNS_DLY_TIMEOUT_SDIO_MIN_MS + 1 );
}


void Adns_WriteBlocking( unsigned char address, unsigned char value )
{
	ADNS_TRIS_MOSI = 0;				// Set MOSI as output
	Adns_WriteSPI( address | ADNS_WRITE_OR_MASK );	// Write the register address
	Adns_WriteSPI( value );			// Write the register value
}


unsigned char Adns_ReadBlocking( unsigned char address )
{
	unsigned char value;
	
	ADNS_TRIS_MOSI = 0;				// Set MOSI as output
	Adns_WriteSPI( address | ADNS_READ_OR_MASK );	// Write the register address
	Adns_ReadSubsequentDelay();		// Used as t_HOLD-SDIO
	
	ADNS_TRIS_MOSI = 1;				// Set MOSI to HiZ
	Adns_AddressDataDelay();
	value = Adns_ReadSPI();			// Read the register value
	
	Adns_ReadSubsequentDelay();		// Used as t_HOLD-READ
	return value;
}


ADNS_BURST_MOTION_DELTAS Adns_BurstReadMotionDeltasBlocking( void )
{
	ADNS_BURST_MOTION_DELTAS result;
	unsigned char	high;
	
	// Call a Motion Burst message chain
	ADNS_TRIS_MOSI = 0;				// Set MOSI as output
	Adns_WriteSPI( ADNS_REG_MOTION_BURST | ADNS_READ_OR_MASK );
	Adns_AddressDataDelay();
	
	// Read the chosen Motion Burst registers
	ADNS_TRIS_MOSI = 1;				// Set MOSI to HiZ
	result.deltaX = Adns_ReadSPI();	// DELTA_X
	result.deltaY = Adns_ReadSPI();	// DELTA_Y
	high = Adns_ReadSPI();			// DELTA_XY_HIGH
	result.deltaY += high & 0x0F;	// Add upper 4 bits
	result.deltaX += high >> 4;
	
	ADNS_TRIS_MOSI = 0;				// Set MOSI as output
	return result;
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


// EOF
