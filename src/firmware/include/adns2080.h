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
// CONFIGURATION

#define	ADNS_USE_INTERRUPT				1					///< Use interrupt instead of polling


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// HEADERS


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL DEFINITIONS

// System properties
#define	ADNS_DPI						   1250				///< Resolution
#define	ADNS_MAX_INCH_SEC				     30				///< Maximum inches per second
#define	ADNS_MAX_SPI_FREQ				1000000				///< Maximum SPI speed [Hz]
#define	ADNS_SPI_FREQ					1000000				///< Actual SPI speed [Hz]

// Delays and timeouts
#define	ADNS_DLY_MOT_RST_MAX_MS			     50				///< t_MOT-RST
#define	ADNS_DLY_REST_EN_MAX_S			      1				///< t_REST-EN
#define	ADNS_DLY_REST_DIS_MAX_S			      1				///< t_REST-DIS
#define	ADNS_DLY_PD_MAX_MS				     50				///< t_PD
#define	ADNS_DLY_WAKEUP_MAX_MS			     55				///< t_WAKEUP
#define	ADNS_DLY_R_SDIO_MAX_NS			    200				///< t_r-SDIO
#define	ADNS_DLY_F_SDIO_MAX_NS			    200				///< t_f-SDIO
#define	ADNS_DLY_DLY_SDIO_MAX_NS		    120				///< t_DLY-SDIO
#define	ADNS_DLY_HOLD_MIN_NS			    250				///< t_hold-SDIO
#define	ADNS_DLY_HOLD_MAX_NS			(1000000000 / ADNS_SPI_FREQ)	///< t_hold-SDIO
#define	ADNS_DLY_TIMEOUT_SDIO_MIN_MS	     50				///< t_timeout-SDIO
#define	ADNS_DLY_TSWR_MIN_MS			     20				///< t_SWR
#define	ADNS_DLY_TSWW_MIN_MS			     30				///< t_SWW
#define	ADNS_DLY_TSRX_MIN_NS			    250				///< t_SRW & t_SRR
#define	ADNS_DLY_TSRAD_MIN_US			      4				///< t_SRAD
#define	ADNS_DLY_SPI_BYTE_US			((8 * 1000000) / ADNS_SPI_FREQ)	///< Delay of a byte sent over SPI

// Pin definitions
#define	ADNS_TRIS_MISO					TRISBbits.TRISB4	///< SPI MISO/SDI
#define	ADNS_TRIS_MOSI					TRISCbits.TRISC7	///< SPI MOSI/SDO
#define	ADNS_TRIS_SCLK					TRISBbits.TRISB6	///< SPI SCLK/SCK
#define	ADNS_TRIS_MOTION				TRISCbits.TRISC2	///< MOTION IRQ

#define	ADNS_PIN_MISO					PORTBbits.RB4		///< SPI MISO/SDI
#define	ADNS_PIN_MOSI					PORTCbits.RC7		///< SPI MOSI/SDO
#define	ADNS_PIN_SCLK					PORTBbits.RB6		///< SPI SCLK/SCK
#define	ADNS_PIN_MOTION					PORTCbits.RC2		///< MOTION IRQ

#define	ADNS_LAT_MISO					LATBbits.LATB4		///< SPI MISO/SDI
#define	ADNS_LAT_MOSI					LATCbits.LATC7		///< SPI MOSI/SDO
#define	ADNS_LAT_SCLK					LATBbits.LATB6		///< SPI SCLK/SCK
#define	ADNS_LAT_MOTION					LATCbits.LATC2		///< MOTION IRQ


// Interrupt settings
#define	ADNS_INT_IE						INTCONbits.INT0IE	///< Interrupt enable
#define	ADNS_INT_IF						INTCONbits.INT0IF	///< Interrupt flag
#define	ADNS_INT_EDGE					INTCON2bits.INTEDG0	///< Interrupt edge register
#define	ADNS_INT_EDGE_VALUE				0					///< Interrupt edge register (1 = rising, 0 = falling)


// Register addresses (see datasheet for description)
#define	ADNS_REG_PROD_ID				0x00				///< Product ID
#define	ADNS_REG_REV_ID					0x01				///< Revision ID
#define	ADNS_REG_MOTION_ST				0x02				///< Motion Status 
#define	ADNS_REG_DELTA_X				0x03				///< Lower byte of Delta_X 
#define	ADNS_REG_DELTA_Y				0x04				///< Lower byte of Delta_Y
#define	ADNS_REG_SQUAL					0x05				///< Squal Quality
#define	ADNS_REG_SHUT_HI				0x06				///< Shutter Open Time (Upper 8-bit) 
#define	ADNS_REG_SHUT_LO				0x07				///< Shutter Open Time (Lower 8-bit)
#define	ADNS_REG_PIX_MAX				0x08				///< Maximum Pixel Value
#define	ADNS_REG_PIX_ACCUM				0x09				///< Average Pixel Value
#define	ADNS_REG_PIX_MIN				0x0A				///< Minimum Pixel Value
#define	ADNS_REG_PIX_GRAB				0x0B				///< Pixel Grabber
#define	ADNS_REG_DELTA_XY_HIGH			0x0C				///< Upper 4 bits of Delta X and Y displacement 
#define	ADNS_REG_MOUSE_CTRL				0x0D				///< Mouse Control
#define	ADNS_REG_RUN_DOWNSHIFT			0x0E				///< Run to Rest1 Time 
#define	ADNS_REG_REST1_PERIOD			0x0F				///< Rest1 Period
#define	ADNS_REG_REST1_DOWNSHIFT		0x10				///< Rest1 to Rest2 Time
#define	ADNS_REG_REST2_PERIOD			0x11				///< Rest2 Period
#define	ADNS_REG_REST2_DOWNSHIFT		0x12				///< Rest2 to Rest3 Time
#define	ADNS_REG_REST3_PERIOD			0x13				///< Rest3 Period
#define	ADNS_REG_PERFORMANCE			0x22				///< Performance
#define	ADNS_REG_RESET					0x3A				///< Reset
#define	ADNS_REG_NOT_REV_ID				0x3F				///< Inverted Revision ID
#define	ADNS_REG_LED_CTRL				0x40				///< LED Control
#define	ADNS_REG_MOTION_CTRL			0x41				///< Motion Control
#define	ADNS_REG_BURST_READ_FIRST		0x42				///< Burst Read Starting Register 
#define	ADNS_REG_BURST_READ_LAST		0x44				///< Burst Read Ending Register
#define	ADNS_REG_REST_MODE_CONFIG		0x45				///< Rest Mode Coni  guration
#define	ADNS_REG_MOTION_BURST			0x63				///< Burst Read


// Register default values (see datasheet)
#define	ADNS_DEF_PROD_ID				0x2A
#define	ADNS_DEF_REV_ID					0x00
#define	ADNS_DEF_MOTION_ST				0x00
#define	ADNS_DEF_DELTA_X				0x00
#define	ADNS_DEF_DELTA_Y				0x00
#define	ADNS_DEF_SQUAL					0x00
#define	ADNS_DEF_SHUT_HI				0x00
#define	ADNS_DEF_SHUT_LO				0x64
#define	ADNS_DEF_PIX_MAX				0xD0
#define	ADNS_DEF_PIX_ACCUM				0x80
#define	ADNS_DEF_PIX_MIN				0x00
#define	ADNS_DEF_PIX_GRAB				0x00
#define	ADNS_DEF_DELTA_XY_HIGH			0x00
#define	ADNS_DEF_MOUSE_CTRL				0x01
#define	ADNS_DEF_RUN_DOWNSHIFT			0x08
#define	ADNS_DEF_REST1_PERIOD			0x01
#define	ADNS_DEF_REST1_DOWNSHIFT		0x1F
#define	ADNS_DEF_REST2_PERIOD			0x09
#define	ADNS_DEF_REST2_DOWNSHIFT		0x2F
#define	ADNS_DEF_REST3_PERIOD			0x31
#define	ADNS_DEF_PERFORMANCE			0x00
#define	ADNS_DEF_RESET					0x00
#define	ADNS_DEF_NOT_REV_ID				0xFF
#define	ADNS_DEF_LED_CTRL				0x00
#define	ADNS_DEF_MOTION_CTRL			0x40
#define	ADNS_DEF_BURST_READ_FIRST		0x03
#define	ADNS_DEF_BURST_READ_LAST		0x09
#define	ADNS_DEF_REST_MODE_CONFIG		0x00
#define	ADNS_DEF_MOTION_BURST			0x00


// Communication bits

#define	ADNS_WRITE_OR_MASK				0x80		///< Value to be OR-ed with the register address, when writing to it
#define	ADNS_READ_OR_MASK				0x00		///< Value to be OR-ed with the register address, when reading from it

#define	ADNS_CMD_RESET					0x5A		///< Write to Reset register to reset


// Register bitfields

typedef union {
	struct {
		unsigned					: 7;
		unsigned	MOTION_ST		: 1;
	} bits;
	unsigned char value;
} ANDS_BITS_MOTION_ST;

typedef union {
	struct {
		unsigned	S				: 4;
		unsigned					: 4;
	} bits;
	unsigned char value;
} ADNS_BITS_SHUT_HI;

typedef union {
	struct {
		unsigned	S				: 8;
	} bits;
	unsigned char value;
} ADNS_BITS_SHUT_LO;

typedef union {
	struct {
		unsigned	PG				: 7;
		unsigned	PG_VALID		: 1;
	} bits;
	unsigned char value;
} ADNS_BITS_PIX_GRAB;

typedef union {
	struct {
		unsigned	DELTA_Y_HI		: 4;
		unsigned	DELTA_X_HI		: 4;
	} bits;
	unsigned char value;
} ADNS_BITS_DELTA_XY_HIGH;

typedef union {
	struct {
		unsigned	RES_D			: 1;
		unsigned	PD				: 1;
		unsigned	RES				: 3;
		unsigned	RES_EN			: 1;
		unsigned					: 1;
		unsigned	BIT_REPORTING	: 1;
	} bits;
	unsigned char value;
} ADNS_BITS_MOUSE_CTRL;

typedef union {
	struct {
		unsigned					: 4;
		unsigned	FORCE			: 3;
		unsigned					: 1;
	} bits;
	unsigned char value;
} ADNS_BITS_PERFORMANCE;

typedef union {
	struct {
		unsigned					: 3;
		unsigned	LCOF			: 1;
		unsigned					: 4;
	} bits;
	unsigned char value;
} ADNS_BITS_LED_CTRL;

typedef union {
	struct {
		unsigned					: 6;
		unsigned	MOT_S			: 1;
		unsigned	MOT_A			: 1;
	} bits;
	unsigned char value;
} ADNS_BITS_MOTION_CTRL;

typedef union {
	struct {
		unsigned					: 6;
		unsigned	RM				: 2;
	} bits;
	unsigned char value;
} ADNS_BITS_REST_MODE_CONFIG;


// Aggregate types of the driver

typedef struct {
	unsigned		motionInt	: 1;	///< Motion flag (interrupt sets, service clears)
	unsigned		dataReady	: 1;	///< Data ready flag
	unsigned					: 6;
} ADNS_STATUS;							///< Device status


typedef struct {
	signed short		dx;				///< X movement since last motion read
	signed short		dy;				///< Y movement since last motion read
} ADNS_DELTAS;							///< Motion deltas


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL VARIABLES

extern near ADNS_STATUS			adns_status;		///< ADNS module status
extern volatile ADNS_DELTAS		adns_deltas;		///< Cached short deltas


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// GLOBAL PROTOTYPES

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
 */
void Adns_Service( void );

/**
 * Motion interrupt callback.
 */
void Adns_MotionCallback( void );


/**
 * Enables the Motion interrupt.
 */
#if ADNS_USE_INTERRUPT
#define	Adns_EnableInterrupt()		{ ADNS_INT_IE = 1; }
#else
#define	Adns_EnableInterrupt()
#endif

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
 * Issues a blocking Burst Read command, in order to retrieve the neded
 * motion deltas.
 *
 * Values are cached into <tt>adns_short_deltas</tt>.
 */
void Adns_BurstReadMotionDeltasBlocking( void );


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


/**
 * Clears the deltas accumulator.
 */
void Adns_ClearDeltas( void );


#endif	/* !__ADNS3530_H__ */
