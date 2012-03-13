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


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL FUNCTIONS
#pragma code code_adns_local

void Adns_WriteSPI( unsigned char value )
{
	volatile unsigned char dummy;
	
	dummy = SSPBUF;					// Clear buffer state
	SSPCON1bits.WCOL = 0;
	PIR1bits.SSPIF = 0;				// Clear the interrupt flag
	
	SSPBUF = value;					// Write the value to be sent
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
	
	SSPBUF = 0xFF;					// Keep MOSI high
//	if ( !SSPCON1bits.WCOL ) {		// Check for no collision
		while ( !PIR1bits.SSPIF );	// Wait until the value is sent
//	}
	return SSPBUF;
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
	ADNS_PIN_MOSI = 0;
	ADNS_PIN_SCLK = 1;
	
	ADNS_TRIS_MISO = 1;
	ADNS_TRIS_MOSI = 0;
	ADNS_TRIS_SCLK = 0;
	ADNS_TRIS_MOTION = 1;
	
	ADNS_PIN_MOSI = 0;
	ADNS_PIN_SCLK = 1;
	
	// Setup Timer 2 time base
	PIE1bits.TMR2IE = 0;			// Disable Timer 2 interrupts
	PIR1bits.TMR2IF = 0;
	T2CON = 0;						// No prescaler, no postscaler, timer disabled
	PR2 = (2 * SYS_FCY) / ADNS_SPI_FREQ;	// Desired time base at TMR2/2
	T2CONbits.TMR2ON = 1;			// Enable the Timer 2 time base
	
	// Configure the SPI module
	SSPSTAT = 0;					// Reset the MSSP status register
	SSPSTATbits.SMP = 0;			// Data valid at half clock period
	SSPSTATbits.CKE = 1;			// Data transition at high->low clock edge
	SSPCON1bits.CKP = 1;			// Idle clock state at high level
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
	
	// Reset the device
	Debug_PrintRom_( "ADNS connection... " );
	Adns_ResetCommunication();
	
	// Check communication
	if ( Adns_CheckCommunication() ) {
		Debug_PrintRom_( "ok\n" );
	} else {
		Debug_PrintRom_( "FAIL\n" );
	}
	
	// Enable MOTION interrupt
	ADNS_INT_IF = 0;
	ADNS_INT_IP = 1;
	ADNS_INT_EDGE = 1;
	Nop();
	Adns_EnableInterrupt();
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
		YELLOW_LED = LED_ON;
		Adns_ResetCommunication();
		burst = Adns_BurstReadMotionDeltasBlocking();
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
	ADNS_TRIS_MISO = 0;
	Adns_WriteSPI( address );
	Adns_WriteSPI( value );
}


unsigned char Adns_ReadBlocking( unsigned char address )
{
	ADNS_TRIS_MISO = 0;
	Adns_WriteSPI( address );
	OneUsDelay();	// FIXME
	ADNS_TRIS_MISO = 1;
	Adns_AddressDataDelay();
	return Adns_ReadSPI();
}


ADNS_BURST_MOTION_DELTAS Adns_BurstReadMotionDeltasBlocking( void )
{
	ADNS_BURST_MOTION_DELTAS result;
	unsigned char	high;
	
	ADNS_TRIS_MISO = 0;
	Adns_WriteSPI( ADNS_REG_MOTION_BURST );
	Adns_AddressDataDelay();
	OneUsDelay();	// FIXME
	ADNS_TRIS_MISO = 1;
	
	result.deltaY = Adns_ReadSPI();
	result.deltaX = Adns_ReadSPI();
	high = Adns_ReadSPI();
	result.deltaY += high & 0x0F;	// Add upper 4 bits
	result.deltaX += high >> 4;
	
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
	Nop();
	Nop();
	Nop();
	Nop();
}


// EOF
