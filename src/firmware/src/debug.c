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

// Baud rate generation
#define	SPBRG_16BITS		0								/// Use a 16-bits counter for BRG
#define	SPBRG_HISPEED		0								/// Use high-speed BRG
#define	SPBRG_SCALE			(						\
	(!SPBRG_16BITS && !SPBRG_HISPEED) ? 64 :		\
	((SPBRG_16BITS != SPBRG_HISPEED) ? 16 : 4) )			/// Divider scale for BRG (automatically determined)
#define	SPBRG_VALUE			((SYS_FOSC / (DEBUG_BAUD_RATE * SPBRG_SCALE)) - 1)	/// SPBRG value

#define	EnableTxInt()		{ DEBUG_UART_INT_TX = 1; }		/// Enables the TX interrupt
#define	DisableTxInt()		{ DEBUG_UART_INT_TX = 0; }		/// Disables the RX interrupt

#define	EnableRxInt()		{ DEBUG_UART_INT_RX = 1; }		/// Enables the TX interrupt
#define	DisableRxInt()		{ DEBUG_UART_INT_RX = 0; }		/// Disables the RX interrupt


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// LOCAL VARIABLES
#pragma udata data_debug_local

volatile DEBUG_UART_BUFFER_INDEX_TYPE	debug_uartTxBufferHead;
volatile DEBUG_UART_BUFFER_INDEX_TYPE	debug_uartTxBufferTail;
volatile unsigned char					debug_uartTxBufferFree;

volatile DEBUG_UART_BUFFER_INDEX_TYPE	debug_uartRxBufferHead;
volatile DEBUG_UART_BUFFER_INDEX_TYPE	debug_uartRxBufferTail;
volatile unsigned char					debug_uartRxBufferFree;

#pragma udata data_debug_txbuf
volatile char							debug_uartTxBufferData[ DEBUG_TX_BUFFER_SIZE ];

#pragma udata data_debug_rxbuf
volatile char							debug_uartRxBufferData[ DEBUG_RX_BUFFER_SIZE ];


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
	RCSTA = 0;					// Reset the receiver configuration
	RCSTAbits.CREN = 1;			// Receive continuously
	
	TXSTA = 0;					// Reset the transmitter configuration
	TXSTAbits.TXEN = 1;			// Enable the transmitter
	
	BAUDCON = 0;
	TXSTAbits.BRGH = 0;			// No need for high-speed debug @ 38400 baud
	BAUDCONbits.BRG16 = 0;		// Use a 8-bits counter
	SPBRG = SPBRG_VALUE;		// Baud rate generator counter
	SPBRGH = (SPBRG_VALUE >> 8);
	
	IPR1bits.TXIP = 0;			// Low-priority interrupts
	IPR1bits.RCIP = 0;
	
	RCSTAbits.SPEN = 1;			// Enable the serial port
	INTCONbits.GIEH = 1;		// Enable interrupts
    INTCONbits.GIEL = 1;
    
	// Print a welcome message
	Debug_PrintRom_( "=> MOUSE SENSOR HID CONTROLLER <=\r\n\r\n" );
	
	// Print some info
    Debug_PrintConst_Initializing();
    Debug_PrintRom_( "UART" );
    Debug_PrintConst_Dots();
    Debug_PrintConst_Ok();
    Debug_PrintConst_NewLine();
    Debug_PrintRom_( "UART SPBRG = 0x" );
    Debug_PrintHex( SPBRGH );
    Debug_PrintHex( SPBRG );
    Debug_PrintConst_NewLine();
    Debug_PrintConst_NewLine();
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
	if ( debug_uartTxBufferFree == DEBUG_TX_BUFFER_SIZE && DEBUG_UART_FLAG_TX ) {
		// SW & HW buffers are free, send directly to the USART
		TXREG = value;
	}
	else {
		// If the buffer is full, block until a character is sent
		if ( debug_uartTxBufferFree == 0 ) {
			while ( !DEBUG_UART_FLAG_TX );
			TXREG = debug_uartTxBufferData[ debug_uartTxBufferHead ];
			if ( ++debug_uartTxBufferHead == DEBUG_TX_BUFFER_SIZE ) {
				debug_uartTxBufferHead = 0;
			}
			++debug_uartTxBufferFree;
		}
		
		// Enqueue the character
		debug_uartTxBufferData[ debug_uartTxBufferTail ] = value;
		if ( ++debug_uartTxBufferTail == DEBUG_TX_BUFFER_SIZE ) {
			debug_uartTxBufferTail = 0;
		}	
		--debug_uartTxBufferFree;
		
		// Process the next character
		EnableTxInt();
	}
}


void Debug_PrintHex( unsigned char value )
{
	unsigned char c = (value >> 4) & 0x0F;
	Debug_PrintChar( c + ((c >= 10) ? ('A' - 10) : '0') );
	c = value & 0x0F;
	Debug_PrintChar( c + ((c >= 10) ? ('A' - 10) : '0') );
}


DEBUG_UART_BUFFER_INDEX_TYPE Debug_TxBufferUsed( void )
{
	return DEBUG_TX_BUFFER_SIZE - debug_uartTxBufferFree;
}


DEBUG_UART_BUFFER_INDEX_TYPE Debug_TxBufferFree( void )
{
	return debug_uartTxBufferFree;
}


DEBUG_UART_BUFFER_INDEX_TYPE Debug_RxBufferUsed( void )
{
	return DEBUG_TX_BUFFER_SIZE - debug_uartRxBufferFree;
}


DEBUG_UART_BUFFER_INDEX_TYPE Debug_RxBufferFree( void )
{
	return debug_uartRxBufferFree;
}


void Debug_Truncate( void )
{
	DisableTxInt();
	debug_uartTxBufferHead = 0;
	debug_uartTxBufferTail = 0;
	debug_uartTxBufferFree = DEBUG_TX_BUFFER_SIZE;
}

	
void Debug_Flush( void )
{
	DisableTxInt();
	while ( debug_uartTxBufferFree < DEBUG_TX_BUFFER_SIZE ) {
		while ( !DEBUG_UART_FLAG_TX );
		TXREG = debug_uartTxBufferData[ debug_uartTxBufferHead ];
		if ( ++debug_uartTxBufferHead == DEBUG_TX_BUFFER_SIZE ) {
			debug_uartTxBufferHead = 0;
		}
		++debug_uartTxBufferFree;
	}
	debug_uartTxBufferHead = 0;
	debug_uartTxBufferTail = 0;
	debug_uartTxBufferFree = DEBUG_TX_BUFFER_SIZE;
}


void Debug_TxIntCallback( void )
{
	// Send the next character
	DisableTxInt();
	if ( debug_uartTxBufferFree < DEBUG_TX_BUFFER_SIZE )
	{
		while ( !DEBUG_UART_FLAG_TX );		// Should always be true
		TXREG = debug_uartTxBufferData[ debug_uartTxBufferHead ];
		if ( ++debug_uartTxBufferHead == DEBUG_TX_BUFFER_SIZE ) {
			debug_uartTxBufferHead = 0;
		}
		++debug_uartTxBufferFree;
		
		// Enable the interrupt if there are queued characters
		if ( debug_uartTxBufferFree < DEBUG_TX_BUFFER_SIZE ) {
			EnableTxInt();
		}	
	}
}


void Debug_RxIntCallback( void )
{
	volatile unsigned char dummy;
	dummy = RCREG;					// Just drop the byte
}


void Debug_PrintConst_NewLine( void )
{
	Debug_PrintChar( '\r' );
	Debug_PrintChar( '\n' );
}


void Debug_PrintConst_Initializing( void )
{
	Debug_PrintRom_( "Initializing " );
}


void Debug_PrintConst_Checking( void )
{
	Debug_PrintRom_( "Checking " );
}


void Debug_PrintConst_EventBegin( void )
{
	Debug_PrintChar( '[' );
	Debug_PrintChar( '@' );
}


void Debug_PrintConst_EventEnd( void )
{
	Debug_PrintChar( ']' );
}


void Debug_PrintConst_Dots( void )
{
	Debug_PrintRom_( "... " );
}


void Debug_PrintConst_Ok( void )
{
	Debug_PrintChar( 'o' );
	Debug_PrintChar( 'k' );
}


void Debug_PrintConst_Fail( void )
{
	Debug_PrintRom_( "FAIL" );
}


void Debug_PrintConst_0x( void )
{
	Debug_PrintChar( '0' );
	Debug_PrintChar( 'x' );
}



// EOF
