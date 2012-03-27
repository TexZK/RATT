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
// CONFIGURATION

#if 1

// Strings
#define	USE_DEBUG_PRINT_ROM
//#define	USE_DEBUG_PRINT_RAM

// Integral values
#define	USE_DEBUG_PRINT_HEX
//#define	USE_DEBUG_PRINT_U8
//#define	USE_DEBUG_PRINT_S8					// Requires U8
//#define	USE_DEBUG_PRINT_U16					// Requires U8
//#define	USE_DEBUG_PRINT_S16					// Requires U16

// Constants in ROM
#define	USE_DEBUG_PRINTCONST_NEWLINE
#define	USE_DEBUG_PRINTCONST_INITIALIZING
#define	USE_DEBUG_PRINTCONST_CHECKING
#define	USE_DEBUG_PRINTCONST_EVENTBEGIN
#define	USE_DEBUG_PRINTCONST_EVENTEND
#define	USE_DEBUG_PRINTCONST_DOTS
#define	USE_DEBUG_PRINTCONST_OK
#define	USE_DEBUG_PRINTCONST_FAIL
#define	USE_DEBUG_PRINTCONST_EQ
#define	USE_DEBUG_PRINTCONST_0X

// Use placeholder when not printing the actual value
#define	USE_DEBUG_PRINTCONST_PLACEHOLDER

#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

#define	DEBUG_BAUD_RATE		38400//115200				/// UART baud rate


// Buffer definitions
#define	DEBUG_TX_BUFFER_BITS		5
#define	DEBUG_TX_BUFFER_SIZE		(1 << DEBUG_TX_BUFFER_BITS)
#define	DEBUG_TX_BUFFER_MASK		(DEBUG_TX_BUFFER_SIZE - 1)

#define	DEBUG_RX_BUFFER_BITS		1
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
#ifndef	DONT_USE_DEBUG_CONSOLE

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
#ifdef	USE_DEBUG_PRINT_ROM
void Debug_PrintRom( const far rom char * text );
#else
#define	Debug_PrintRom( text )			Debug_PrintPlaceholder()
#endif


/**
 * Prints a string from ROM, with pointer casting.
 * Useful to print string literals.
 *
 * @param text
 *		ROM pointer to the string to be printed
 */
#define	Debug_PrintRom_( text )			Debug_PrintRom( (const far rom char *)(text) )


/**
 * Prints a string from RAM.
 */
#ifdef	USE_DEBUG_PRINT_RAM
void Debug_PrintRam( const far ram char * text );
#else
#define	Debug_PrintRam( text )			Debug_PrintPlaceholder()
#endif


/**
 * Prints a string from RAM, with pointer casting.
 * Useful to print string literals.
 *
 * @param text
 *		RAM pointer to the string to be printed
 */
#define	Debug_PrintRam_( text )			Debug_PrintRam( (const far ram char *)(text) )


/**
 * Prints a character.
 */
void Debug_PrintChar( char value );


/**
 * Prints a HEX byte.
 *
 * @param value
 *		Value to print.
 */
#ifdef	USE_DEBUG_PRINT_HEX
void Debug_PrintHex( unsigned char value );
#else
#define	Debug_PrintHex( value )			Debug_PrintPlaceholder()
#endif


/**
 * Prints an unsigned 8-bits value.
 *
 * @param value
 *		Value to print.
 */
#ifdef	USE_DEBUG_PRINT_U8
void Debug_PrintU8( unsigned char value );
#else
#define	Debug_PrintU8( value )			Debug_PrintPlaceholder()
#endif


/**
 * Prints an unsigned 8-bits value.
 *
 * @param value
 *		Value to print.
 */
#ifdef	USE_DEBUG_PRINT_U16
void Debug_PrintU16( unsigned short value );
#else
#define	Debug_PrintU16( value )			Debug_PrintPlaceholder()
#endif


/**
 * Prints a signed 8-bits value.
 *
 * @param value
 *		Value to print.
 */
#ifdef	USE_DEBUG_PRINT_S8
void Debug_PrintS8( signed char value );
#else
#define	Debug_PrintS8( value )			Debug_PrintPlaceholder()
#endif


/**
 * Prints a signed 16-bits value.
 *
 * @param value
 *		Value to print.
 */
#ifdef	USE_DEBUG_PRINT_S16
void Debug_PrintS16( signed short value );
#else
#define	Debug_PrintS16( value )			Debug_PrintPlaceholder()
#endif


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
 * Prints '#'.
 */
#ifdef	USE_DEBUG_PRINTCONST_PLACEHOLDER
void Debug_PrintPlaceholder( void );
#else
#define	Debug_PrintPlaceholder()
#endif


/**
 * Prints "\r\n".
 */
#ifdef	USE_DEBUG_PRINTCONST_NEWLINE
void Debug_PrintConst_NewLine( void );
#else
#define	Debug_PrintConst_NewLine()			Debug_PrintPlaceholder()
#endif



/**
 * Prints "Initializing ".
 */
#ifdef	USE_DEBUG_PRINTCONST_INITIALIZING
void Debug_PrintConst_Initializing( void );
#else
#define	Debug_PrintConst_Initializing()		Debug_PrintPlaceholder()
#endif


/**
 * Prints "Checking ".
 */
#ifdef	USE_DEBUG_PRINTCONST_CHECKING
void Debug_PrintConst_Checking( void );
#else
#define	Debug_PrintConst_Checking()			Debug_PrintPlaceholder()
#endif


/**
 * Prints "[@" (begins event info string).
 */
#ifdef	USE_DEBUG_PRINTCONST_EVENTBEGIN
void Debug_PrintConst_EventBegin( void );
#else
#define	Debug_PrintConst_EventBegin()		Debug_PrintPlaceholder()
#endif


/**
 * Prints "]" (ends event info string).
 */
#ifdef	USE_DEBUG_PRINTCONST_EVENTEND
void Debug_PrintConst_EventEnd( void );
#else
#define	Debug_PrintConst_EventEnd()			Debug_PrintPlaceholder()
#endif


/**
 * Prints "... ".
 */
#ifdef	USE_DEBUG_PRINTCONST_DOTS
void Debug_PrintConst_Dots( void );
#else
#define	Debug_PrintConst_Dots()				Debug_PrintPlaceholder()
#endif


/**
 * Prints "ok".
 */
#ifdef	USE_DEBUG_PRINTCONST_OK
void Debug_PrintConst_Ok( void );
#else
#define	Debug_PrintConst_Ok()				Debug_PrintPlaceholder()
#endif


/**
 * Prints "FAIL".
 */
#ifdef	USE_DEBUG_PRINTCONST_FAIL
void Debug_PrintConst_Fail( void );
#else
#define	Debug_PrintConst_Fail()				Debug_PrintPlaceholder()
#endif


/**
 * Prints " = ".
 */
#ifdef	USE_DEBUG_PRINTCONST_EQ
void Debug_PrintConst_Eq( void );
#else
#define	Debug_PrintConst_Eq()				Debug_PrintPlaceholder()
#endif


/**
 * Prints "0x".
 */
#ifdef	USE_DEBUG_PRINTCONST_0X
void Debug_PrintConst_0x( void );
#else
#define	Debug_PrintConst_0x()				Debug_PrintPlaceholder()
#endif


#else	/* DONT_USE_DEBUG_CONSOLE */

// Replace with empty symbols, since we are entirely skipping the console
#define Debug_Initialize()
#define	Debug_Release()
#define Debug_PrintRom( text )
#define	Debug_PrintRom_( text )
#define	Debug_PrintRam( text )
#define	Debug_PrintRam_( text )
#define	Debug_PrintChar( value )
#define	Debug_PrintHex( value )
#define	Debug_PrintU8( value )
#define	Debug_PrintU16( value )
#define	Debug_PrintS8( value )
#define	Debug_PrintS16( value )
#define	Debug_TxBufferUsed()
#define	Debug_TxBufferFree()
#define	Debug_RxBufferUsed()
#define	Debug_RxBufferFree()
#define	Debug_Truncate()
#define	Debug_Flush()
#define	Debug_TxIntCallback()
#define	Debug_RxIntCallback()
#define	Debug_PrintConst_NewLine()
#define	Debug_PrintConst_Initializing()
#define	Debug_PrintConst_Checking()
#define	Debug_PrintConst_EventBegin()
#define	Debug_PrintConst_EventEnd()
#define	Debug_PrintConst_Dots()
#define	Debug_PrintConst_Ok()
#define	Debug_PrintConst_Fail()
#define	Debug_PrintConst_Eq()
#define	Debug_PrintConst_0x()


#endif	/* ifndef DONT_USE_DEBUG_CONSOLE */
#endif	/* !__DEBUG_H__ */
