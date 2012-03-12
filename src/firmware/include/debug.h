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

#define	DEBUG_UART_BAUD_RATE		115200				/// UART baud rate
#define	DEBUG_UART_BUFFER_BITS		6
#define	DEBUG_UART_BUFFER_SIZE		(1 << DEBUG_UART_BUFFER_BITS)
#define	DEBUG_UART_BUFFER_MASK		(DEBUG_UART_BUFFER_SIZE - 1)

#if ( DEBUG_UART_BUFFER_BITS <= 7 )
typedef unsigned char	DEBUG_UART_BUFFER_INDEX_TYPE;
#else
typedef unsigned short	DEBUG_UART_BUFFER_INDEX_TYPE;
#endif

#define	DEBUG_UART_INT_TX			PIE1bits.TXIE		/// TX interrupt switch
#define	DEBUG_UART_INT_RX			PIE1bits.RCIE		/// RX interrupt switch
#define	DEBUG_UART_FLAG_TX			PIR1bits.TXIF		/// TX interrupt flag
#define	DEBUG_UART_FLAG_RX			PIR1bits.RCIF		/// RX interrupt flag

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
 */
void Debug_PrintRom( const far rom char * text );


/**
 * Prints a string from RAM.
 */
void Debug_PrintRam( const far ram char * text );


/**
 * Prints a character.
 */
void Debug_PrintChar( char value );


/**
 * Prints a HEX byte.
 */
void Debug_PrintByte( unsigned char value );


/**
 * @return
 *		The number of buffer bytes used.
 */
DEBUG_UART_BUFFER_INDEX_TYPE Debug_BufferUsed( void );


/**
 * @return
 *		The number of buffer bytes free.
 */
DEBUG_UART_BUFFER_INDEX_TYPE Debug_BufferFree( void );


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


#endif	/* !__DEBUG_H__ */
