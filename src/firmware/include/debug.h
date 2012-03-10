/**
 * Debug over UART.
 *
 * This file contains the debug functions, so that messages are sent through
 * the UART port.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

#ifndef __DEBUG_H__
#define	__DEBUG_H__


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

#define	DEBUG_UART_BAUD_RATE		115200		/// UART baud rate
#define	DEBUG_UART_BUFFER_BITS		6
#define	DEBUG_UART_BUFFER_SIZE		(1 << DEBUG_UART_BUFFER_BITS)
#define	DEBUG_UART_BUFFER_MASK		(DEBUG_UART_BUFFER_SIZE - 1)

#if ( DEBUG_UART_BUFFER_BITS <= 7 )
typedef unsigned char	DEBUG_UART_BUFFER_INDEX_TYPE;
#else
typedef unsigned short	DEBUG_UART_BUFFER_INDEX_TYPE;
#endif

#define	DEBUG_UART_TX_INT			PIE1bits.TXIE
#define	DEBUG_UART_TX_FLAG			PIR1bits.TXIF
#define	DEBUG_UART_RX_INT			PIE1bits.RCIE
#define	DEBUG_UART_RX_FLAG			PIR1bits.RCIF


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata data_debug_global


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES
#pragma code code_debug_global

// TODO: Docs!

void Debug_Initialize( void );
void Debug_Release( void );

void Debug_PrintRom( const far rom char * text );
void Debug_PrintRam( const far ram char * text );

void Debug_PrintChar( char value );
void Debug_PrintByte( unsigned char value );

DEBUG_UART_BUFFER_INDEX_TYPE Debug_BufferUsed( void );
DEBUG_UART_BUFFER_INDEX_TYPE Debug_BufferFree( void );
void Debug_Truncate( void );
void Debug_Flush( void );

void Debug_TxIntCallback( void );
void Debug_RxIntCallback( void );


#pragma code
#endif	/* !__DEBUG_H__ */
