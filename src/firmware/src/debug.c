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
#include "app.h"
#include "debug.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL DEFINITIONS
#pragma udata data_debug_local

/**
 * Value of the SPBRG register (baud rate generator), supposing an 8-bit counter
 * and no need for high-speed transmission.
 * [value = 77]
 */
#define	SPBRG_VALUE			((SYS_FOSC / (DEBUG_UART_BAUD_RATE * 16)) - 1)

#define	EnableTxInt()		{ DEBUG_UART_INT_TX = 1; }		/// Enables the TX interrupt
#define	DisableTxInt()		{ DEBUG_UART_INT_TX = 0; }		/// Disables the RX interrupt

#define	EnableRxInt()		{ DEBUG_UART_INT_RX = 1; }		/// Enables the TX interrupt
#define	DisableRxInt()		{ DEBUG_UART_INT_RX = 0; }		/// Disables the RX interrupt


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
	Debug_Release();			// Release the module
	
	// Configure the UART module
	SPBRG = SPBRG_VALUE;		// Baud rate generator counter
	SPBRGH = (SPBRG_VALUE >> 8);
	
	RCSTA = 0;					// Reset the receiver configuration
	RCSTAbits.CREN = 1;			// Receive continuously
	
	TXSTA = 0;					// Reset the transmitter configuration
	TXSTAbits.TXEN = 1;			// Enable the transmitter
	
	IPR1bits.TXIP = 0;			// Low-priority interrupts
	IPR1bits.RCIP = 0;
	
	RCSTAbits.SPEN = 1;			// Enable the serial port
}

	
void Debug_Release( void )
{
	Debug_Truncate();			// Free the buffers
	DisableTxInt();				// Disable interrupts
	DisableRxInt();
	RCSTAbits.SPEN = 0;			// Turn the UART off
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
	if ( debug_uartBufferFree == DEBUG_UART_BUFFER_SIZE && DEBUG_UART_FLAG_TX ) {
		// SW & HW buffers are free, send directly to the USART
		TXREG = value;
	}
	else {
		// If the buffer is full, block until a character is sent
		if ( debug_uartBufferFree == 0 ) {
			while ( !DEBUG_UART_FLAG_TX );
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
		while ( !DEBUG_UART_FLAG_TX );
		TXREG = debug_uartBufferData[ debug_uartBufferHead ];
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
		while ( !DEBUG_UART_FLAG_TX );		// Should always be true
		TXREG = debug_uartBufferData[ debug_uartBufferHead ];
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
