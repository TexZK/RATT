/**
 * PAN3101DB library.
 *
 * This file contains the debug functions, so that messages are sent through
 * the UART port.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS

#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"

#include "debug.h"

#include <usart.h>
#include <stdlib.h>


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL DEFINITIONS
#pragma udata data_debug_local

#define	EnableTxInt()		{ DEBUG_UART_TX_INT = 1; }
#define	DisableTxInt()		{ DEBUG_UART_TX_INT = 0; }

#define	EnableRxInt()		{ DEBUG_UART_RX_INT = 1; }
#define	DisableRxInt()		{ DEBUG_UART_RX_INT = 0; }


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_debug_local

volatile char							debug_uartBufferData[ DEBUG_UART_BUFFER_SIZE ];
volatile DEBUG_UART_BUFFER_INDEX_TYPE	debug_uartBufferHead;
volatile DEBUG_UART_BUFFER_INDEX_TYPE	debug_uartBufferTail;
volatile unsigned char					debug_uartBufferFree;


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata data_debug_global


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL PROTOTYPES
#pragma code code_debug_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL FUNCTIONS
#pragma code code_debug_local


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL FUNCTIONS
#pragma code code_debug_global

void Debug_Initialize( void )
{
	// Release the module
	Debug_Release();
	
	// Open the UART port
	OpenUSART(
		USART_TX_INT_OFF |
		USART_RX_INT_OFF |
		USART_ASYNCH_MODE |
		USART_EIGHT_BIT |
		USART_CONT_RX |
		USART_BRGH_HIGH,
		(CLOCK_FREQ / (DEBUG_UART_BAUD_RATE * 16)) - 1
	);
	IPR1bits.TXIP = 0;	// Low-priority interrupts
	IPR1bits.RCIP = 0;
	
	DisableTxInt();
	DisableRxInt();
}

	
void Debug_Release( void )
{
	// Disable interrupts and free the buffer
	Debug_Truncate();
	CloseUSART();
}


void Debug_PrintRom( const far rom char * text )
{
	while ( *text ) {
		Debug_PrintChar( *text );
		++text;
	}
}


void Debug_PrintRam( const far ram char * text )
{
	while ( *text ) {
		Debug_PrintChar( *text );
		++text;
	}
}


void Debug_PrintChar( char value )
{
	DisableTxInt();
	if ( debug_uartBufferFree == DEBUG_UART_BUFFER_SIZE && DEBUG_UART_TX_FLAG ) {
		// SW & HW buffers are free, send directly to the USART
		WriteUSART( value );
	}
	else {
		// If the buffer is full, block until a character is sent
		if ( debug_uartBufferFree == 0 ) {
			while ( !DEBUG_UART_TX_FLAG );
			++debug_uartBufferHead;
			debug_uartBufferHead &= DEBUG_UART_BUFFER_MASK;
			++debug_uartBufferFree;
		}
		
		// Enqueue the character
		debug_uartBufferData[ debug_uartBufferTail ] = value;
		if ( ++debug_uartBufferTail == DEBUG_UART_BUFFER_SIZE ) {
			debug_uartBufferTail = 0;
		}	
		--debug_uartBufferFree;
		
		// Process the next character
		EnableTxInt();
	}
}


void Debug_PrintByte( unsigned char value )
{
	unsigned char c = value & 0x0F;
	Debug_PrintChar( (c > 9) ? (c + 'A') : (c + '0') );
	c = value >> 4;
	Debug_PrintChar( (c > 9) ? (c + 'A') : (c + '0') );
}


DEBUG_UART_BUFFER_INDEX_TYPE Debug_BufferUsed( void )
{
	return DEBUG_UART_BUFFER_SIZE - debug_uartBufferFree;
}


DEBUG_UART_BUFFER_INDEX_TYPE Debug_BufferFree( void )
{
	return debug_uartBufferFree;
}


void Debug_Truncate( void )
{
	DisableTxInt();
	debug_uartBufferHead = 0;
	debug_uartBufferTail = 0;
	debug_uartBufferFree = DEBUG_UART_BUFFER_SIZE;
}

	
void Debug_Flush( void )
{
	DisableTxInt();
	while ( debug_uartBufferFree < DEBUG_UART_BUFFER_SIZE ) {
		while ( !DEBUG_UART_TX_FLAG );
		WriteUSART( debug_uartBufferData[ debug_uartBufferHead ] );
		++debug_uartBufferHead;
		debug_uartBufferHead &= DEBUG_UART_BUFFER_MASK;
		++debug_uartBufferFree;
	}
	debug_uartBufferHead = 0;
	debug_uartBufferTail = 0;
	debug_uartBufferFree = DEBUG_UART_BUFFER_SIZE;
}


void Debug_TxIntCallback( void )
{
	// Send the next character
	DisableTxInt();
	if ( debug_uartBufferFree < DEBUG_UART_BUFFER_SIZE )
	{
		while ( !DEBUG_UART_TX_FLAG );		// Should always be true
		WriteUSART( debug_uartBufferData[ debug_uartBufferHead ] );
		++debug_uartBufferHead;
		debug_uartBufferHead &= DEBUG_UART_BUFFER_MASK;
		++debug_uartBufferFree;
		
		// Enable the interrupt if there are queued characters
		if ( debug_uartBufferFree < DEBUG_UART_BUFFER_SIZE ) {
			EnableTxInt();
		}	
	}
}


void Debug_RxIntCallback( void )
{
	// Receiver not used
}



// EOF
