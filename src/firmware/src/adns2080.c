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

#include "app.h"
#include "TimeDelay.h"
#include "usb/usb_user.h"
#include "adns2080.h"

#include <spi.h>	// FIXME: Hand-written code works better >.<


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL DEFINITIONS
#pragma udata data_adns_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_adns_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata access data_adns_global_access

near ADNS_STATUS	adns_status;		/// Device status.


#pragma udata data_adns_global

signed char			adns_deltaY;		/// Copy of Delta_Y register been read.
signed char			adns_deltaX;		/// Copy of Delta_X register been read.

signed short		adns_y;				/// Current Y position.
signed short		adns_x;				/// Current X position.


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL PROTOTYPES
#pragma code code_adns_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL FUNCTIONS
#pragma code code_adns_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL FUNCTIONS
#pragma code code_adns_global

void Adns_Initialize( void )
{
	// Disable SPI and interrupts
	DisableIntSPI;
	SetPriorityIntSPI( 0 );		// Low priority, but unused
	CloseSPI();
	SPI_Clear_Intr_Status_Bit;
	
	// Reset variables
	adns_status.motionInt = 0;
	adns_status.dataReady = 0;
	
	adns_deltaX = 0;
	adns_deltaY = 0;
	adns_x = 0;
	adns_y = 0;
	
	// Setup pins
	ADNS_PIN_MOSI = 0;
	ADNS_PIN_SCLK = 1;
	
	ADNS_TRIS_MISO = 1;
	ADNS_TRIS_MOSI = 0;
	ADNS_TRIS_SCLK = 0;
	ADNS_TRIS_MOTION = 1;
	
	ADNS_PIN_MOSI = 0;
	ADNS_PIN_SCLK = 1;
	
	// Open SPI port
	OpenSPI(
		SPI_FOSC_64,			// 750kHz @ 48MHz
		MODE_10,				// CKP=1, CKE=1
		SMPMID					// Sample at half-period
	);
	
	// TODO: Reset the device
	Adns_ResetCommunication();
	
	// Enable MOTION interrupt
	Adns_DisableInterrupt();
	ADNS_INT_IF = 0;
	ADNS_INT_IP = 1;
	ADNS_INT_EDGE = 1;
	Nop();
	Adns_EnableInterrupt();
}


void Adns_PowerUpDelay( void )
{
	DelayMs( ADNS_POWERUP_MS );
}


void Adns_Service( void )
{
	// Check for motion interrupt
	if ( adns_status.motionInt ) {
		ADNS_BURST_MOTION_DELTAS burst;
		
		Adns_DisableInterrupt();
		adns_status.motionInt = 0;		// Clear the motion flag
		
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
			ADNS_HID_TX_DATA data;
			
			// Build the packet
			Adns_DisableInterrupt();
			data.deltaX = adns_x;
			data.deltaY = adns_y;
			Adns_EnableInterrupt();
			*((ADNS_HID_TX_DATA*)usb_txBuffer) = data;
			
			// Send the HID packet
			Usb_TxBufferedPacket();
			
			// Clear the delta state
			Adns_DisableInterrupt();
			adns_status.dataReady = 0;
			adns_x = 0;
			adns_y = 0;
			Adns_EnableInterrupt();
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
#if 0	// TODO
	if ( Adns_ReadBlocking( ADNS_REG_PRODUCT_ID ) != ADNS_DEF_PRODUCT_ID ) {
		return 0;
	}
	Adns_ReadSubsequentDelay();
	
	if ( Adns_ReadBlocking( ADNS_REG_REVISION_ID ) != ADNS_DEF_REVISION_ID ) {
		return 0;
	}
	Adns_ReadSubsequentDelay();
	
	if ( Adns_ReadBlocking( ADNS_REG_INV_PRODUCT_ID ) != ADNS_DEF_INV_PRODUCT_ID ) {
		return 0;
	}
	Adns_ReadSubsequentDelay();
	
	if ( Adns_ReadBlocking( ADNS_REG_INV_REVISION_ID ) != ADNS_DEF_INV_REVISION_ID ) {
		return 0;
	}
#endif
	return 1;
}


void Adns_ResetCommunication( void )
{
	// TODO
}


void Adns_WriteBlocking( unsigned char address, unsigned char value )
{
	ADNS_TRIS_MISO = 0;
	WriteSPI( address );
	WriteSPI( value );
}


unsigned char Adns_ReadBlocking( unsigned char address )
{
	ADNS_TRIS_MISO = 0;
	WriteSPI( address );
	OneUsDelay();	// FIXME
	ADNS_TRIS_MISO = 1;
	Adns_AddressDataDelay();
	return ReadSPI();
}


ADNS_BURST_MOTION_DELTAS Adns_BurstReadMotionDeltasBlocking( void )
{
	ADNS_BURST_MOTION_DELTAS result;
	
	ADNS_TRIS_MISO = 0;
	//WriteSPI( ADNS_REG_MOTION_BURST ); // TODO
	Adns_AddressDataDelay();
	OneUsDelay();	// FIXME
	ADNS_TRIS_MISO = 1;
	
	result.deltaY = ReadSPI();
	result.deltaX = ReadSPI();
	
	return result;
}	


void Adns_WriteWriteDelay( void )
{
	Delay10us( (30 + 9 - 2 * ADNS_SPI_BYTE_DELAY) / 10 );
}


void Adns_WriteReadDelay( void )
{
	Delay10us( (20 + 9 - ADNS_SPI_BYTE_DELAY) / 10 );
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


void Adns_WriteInactiveDelay( void )
{
	Delay10us( 20 / 10 );
}	


void Adns_ReadInactiveDelay( void )
{
	Nop();
}


// EOF
