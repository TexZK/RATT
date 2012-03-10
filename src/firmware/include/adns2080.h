/**
 * Avago ADNS-2080 driver.
 *
 * This file contains the function calls in order to communicate to an
 * Avago ADNS-2080 through the SPI port of the PIC18LF14K50.
 *
 * @author Andrea "TexZK" Zoppi
 *
 */

#ifndef __ADNS2080_H__
#define	__ADNS2080_H__


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

// System properties
#define	ADNS_DPI						   1000				/// Resolution
#define	ADNS_INCH_SEC					     30				/// Inches per second
#define	ADNS_POWERUP_MS					     50				/// Power-up delay in millseconds
#define	ADNS_MAX_SPI_FREQ				1000000				/// Maximum SPI speed [Hz]
#define	ADNS_SPI_FREQ					 750000				/// Actual SPI speed [Hz] (see SPI setup)

#define	ADNS_SPI_BYTE_DELAY				((8 * 1000000) / ADNS_SPI_FREQ)	/// Microseconds delay of a byte sent over SPI

// Pin definitions
#define	ADNS_PIN_MISO					PORTBbits.RB4		/// SPI MISO/SDO
#define	ADNS_PIN_MOSI					PORTCbits.RC7		/// SPI MOSI/SDI
#define	ADNS_PIN_SCLK					PORTBbits.RB6		/// SPI SCLK/SCK
#define	ADNS_PIN_MOTION					PORTCbits.RC2		/// ADNS motion interrupt request

#define	ADNS_TRIS_MISO					TRISBbits.TRISB4	/// SPI MISO/SDO
#define	ADNS_TRIS_MOSI					TRISCbits.TRISC7	/// SPI MOSI/SDI
#define	ADNS_TRIS_SCLK					TRISBbits.TRISB6	/// SPI SCLK/SCK
#define	ADNS_TRIS_MOTION				TRISCbits.TRISC2	/// ADNS motion interrupt request


// Interrupt settings
#define	ADNS_INT_IE						INTCON3bits.INT2IE	/// Interrupt enable
#define	ADNS_INT_IF						INTCON3bits.INT2IF	/// Interrupt flag
#define	ADNS_INT_IP						INTCON3bits.INT2IP	/// Interrupt priority (1 = high, 0 = low)
#define	ADNS_INT_EDGE					INTCON2bits.INTEDG2	/// Interrupt edge (1 = rising, 0 = falling)

// Register addresses
#define	ADNS_REG_				0x00				/// 


// Register default values
#define	ADNS_DEF_				


// Bitfields and aggregate types

typedef struct {
	unsigned	motionInt	: 1;	/// Motion flag (interrupt sets, service clears)
	unsigned	dataReady	: 1;	/// Data ready flag
	unsigned				: 6;
} ADNS_STATUS;						/// Device status


typedef union {
	ADNS_BITS_MOTION	bits;
	unsigned char		Val;
} ADNS_MOTION;						/// Motion Burst register, with bits and byte value


typedef struct {
	ADNS_MOTION	motion;
	signed char	deltaY;
	signed char	deltaX;
} ADNS_BURST_MOTION_DELTAS;			/// Motion burst registers: motion, delta Y, delta X


typedef struct {
	signed short	deltaX;			/// X movement since last HID report
	signed short	deltaY;			/// Y movement since last HID report
} ADNS_HID_TX_DATA;					/// HID data for USB transfers	


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES
#pragma udata access data_adns_global_access

extern near ADNS_STATUS	adns_status;


#pragma udata data_adns_global

extern signed char		adns_lastDeltaX;
extern signed char		adns_lastDeltaY;

extern signed short		adns_x;
extern signed short		adns_y;


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES
#pragma code code_adns_global

/**
 * Initializes the device service, and the SPI module.
 */
void Adns_Initialize( void );


/**
 * Generates a delay after power-up, so the device will be working
 * correctly.
 */
void Adns_PowerUpDelay( void );


/**
 * Service which handles all the device management and communication.
 * Usually called only in response to SPI interrupts.
 */
void Adns_Service( void );


/**
 * Motion interrupt callback.
 */
void Adns_MotionCallback( void );


/**
 * Enables the Motion interrupt.
 */
#define	Adns_EnableInterrupt()		{ ADNS_INT_IE = 1; }


/**
 * Disables the Motion interrupt.
 */
#define	Adns_DisableInterrupt()		{ ADNS_INT_IE = 0; }


/**
 * Checks if the communication channel is working properly, by reading
 * PID values and comparing them with known values. Each call changes
 * the PID being read.
 *
 * @return Non-zero <=> channel is valid.
 */
unsigned char Adns_CheckCommunication( void );


/**
 * Forces a communication reset.
 */
void Adns_ResetCommunication( void );


/**
 * Writes a value to SPI, blocking the program.
 *
 * @param address
 *		Register address.
 * @param value
 *		Register value.
 */
void Adns_WriteBlocking( unsigned char address, unsigned char value );


/** 
 * Reads a value from SPI, blocking the program.
 *
 * @param address
 *		Register address.
 */
unsigned char Adns_ReadBlocking( unsigned char address );


/**
 * Issues a blocking Burst Read command, in order to retrieve the first
 * 3 registers of the Motion Burst.
 *
 * @return
 *		The first 3 registers of the Motion Burst.
 */
ADNS_BURST_MOTION_DELTAS Adns_BurstReadMotionDeltasBlocking( void );


/**
 * Generates a delay between Write->Write commands.
 */
void Adns_WriteWriteDelay( void );


/**
 * Generates a delay between Write->Read commands.
 */
void Adns_WriteReadDelay( void );


/**
 * Generates a delay between Address and Data.
 */
void Adns_AddressDataDelay( void );


/**
 * Generates a delay between a Read and subsequent commands (either
 * Read or Write).
 */
void Adns_ReadSubsequentDelay( void );


#pragma code
#endif	/* !__ADNS3530_H__ */
