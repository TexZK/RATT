
#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER	

#define CLOCK_FREQ				48000000
#define	GetInstructionClock()	(CLOCK_FREQ / 4)

// 1us delay @ 48 MHz (12 MIPS => 12 NOPs)
#define OneUsDelayPrecise()	{Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();}
void OneUsDelay( void );

#define	self_power 0


// TODO: Move away
extern unsigned char usb_txBuffer[64];
extern unsigned char usb_rxBuffer[64];
void USBTxBufferedPacket( void );
void USBRxBufferedPacket( void );
unsigned char USBTxReady( void );
unsigned char USBRxReady( void );


#endif
