/**
 * Debug console over UART.
 *
 * This file contains the debug functions, so that messages are sent through
 * the UART port.
 *
 * @author Andrea "TexZK" Zoppi
 */

#ifndef __DEBUG_H__
#define	__DEBUG_H__


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

#define	DEBUG_BAUD_RATE		38400//115200				/// UART baud rate


// Buffer definitions
#define	DEBUG_TX_BUFFER_BITS		5
#define	DEBUG_TX_BUFFER_SIZE		(1 << DEBUG_TX_BUFFER_BITS)
#define	DEBUG_TX_BUFFER_MASK		(DEBUG_TX_BUFFER_SIZE - 1)

#define	DEBUG_RX_BUFFER_BITS		5
#define	DEBUG_RX_BUFFER_SIZE		(1 << DEBUG_RX_BUFFER_BITS)
#define	DEBUG_RX_BUFFER_MASK		(DEBUG_RX_BUFFER_SIZE - 1)

#if ( DEBUG_UART_BUFFER_BITS <= 7 )
typedef unsigned char	DEBUG_UART_BUFFER_INDEX_TYPE;
#else
typedef unsigned short	DEBUG_UART_BUFFER_INDEX_TYPE;
#endif


// Interrupt definitions
#define	DEBUG_UART_INT_TX			PIE1bits.TXIE		/// TX interrupt switch
#define	DEBUG_UART_INT_RX			PIE1bits.RCIE		/// RX interrupt switch
#define	DEBUG_UART_FLAG_TX			PIR1bits.TXIF		/// TX interrupt flag
#define	DEBUG_UART_FLAG_RX			PIR1bits.RCIF		/// RX interrupt flag


// Pin definitions
#define	DEBUG_TRIS_TX				TRISBbits.TRISB7	/// TX tristate
#define	DEBUG_TRIS_RX				TRISBbits.TRISB5	/// RX tristate
#define	DEBUG_PIN_TX				PORTBbits.RB7		/// TX pin
#define	DEBUG_PIN_RX				PORTBbits.RB5		/// RX pin


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES

/**
 * Initializes the debug module (UART and buffers).
 */
void Debug_Initialize( void );


/**
 * Releases the debug module.
 */
void Debug_Release( void );


/**
 * Prints a string from ROM.
 *
 * @param text
 *		ROM pointer to the string to be printed
 */
void Debug_PrintRom( const far rom char * text );

/**
 * Prints a string from ROM, with pointer casting.
 * Useful to print string literals.
 *
 * @param text
 *		ROM pointer to the string to be printed
 */
#define	Debug_PrintRom_( text )		Debug_PrintRom( (const far rom char *)(text) )

/**
 * Prints a string from RAM.
 */
void Debug_PrintRam( const far ram char * text );

/**
 * Prints a string from RAM, with pointer casting.
 * Useful to print string literals.
 *
 * @param text
 *		RAM pointer to the string to be printed
 */
#define	Debug_PrintRam_( text )		Debug_PrintRam( (const far ram char *)(text) )


/**
 * Prints a character.
 */
void Debug_PrintChar( char value );


/**
 * Prints a HEX byte.
 */
void Debug_PrintHex( unsigned char value );


/**
 * @return
 *		The number of TX buffer bytes used.
 */
DEBUG_UART_BUFFER_INDEX_TYPE Debug_TxBufferUsed( void );


/**
 * @return
 *		The number of TX buffer bytes free.
 */
DEBUG_UART_BUFFER_INDEX_TYPE Debug_TxBufferFree( void );


/**
 * @return
 *		The number of RX buffer bytes used.
 */
DEBUG_UART_BUFFER_INDEX_TYPE Debug_RxBufferUsed( void );


/**
 * @return
 *		The number of RX buffer bytes free.
 */
DEBUG_UART_BUFFER_INDEX_TYPE Debug_RxBufferFree( void );


/**
 * Immediately drops the communication and resets buffers.
 */
void Debug_Truncate( void );


/**
 * Waits until all the buffered bytes are sent.
 */
void Debug_Flush( void );


/**
 * Callback of byte been transmitted over UART.
 */
void Debug_TxIntCallback( void );


/**
 * Callback of byte been received by UART.
 */
void Debug_RxIntCallback( void );


// Common constant strings, to reduce the occupied ROM space

/**
 * Prints "\r\n".
 */
void Debug_PrintConst_NewLine( void );


/**
 * Prints "Initializing ".
 */
void Debug_PrintConst_Initializing( void );


/**
 * Prints "Checking ".
 */
void Debug_PrintConst_Checking( void );


/**
 * Prints "[@" (begins event info string).
 */
void Debug_PrintConst_EventBegin( void );


/**
 * Prints "]" (ends event info string).
 */
void Debug_PrintConst_EventEnd( void );


/**
 * Prints "... ".
 */
void Debug_PrintConst_Dots( void );


/**
 * Prints "ok".
 */
void Debug_PrintConst_Ok( void );


/**
 * Prints "FAIL".
 */
void Debug_PrintConst_Fail( void );


/**
 * Prints "0x".
 */
void Debug_PrintConst_0x( void );


#endif	/* !__DEBUG_H__ */
