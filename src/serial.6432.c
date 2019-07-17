#include "common.h"
#include "serial.h"
#include "sym6432.h"

#define COM1 0x03f8   /* COM1 */
#define COM2 0x02f8   /* COM1 */
#define COM3 0x03e8   /* COM1 */
#define COM4 0x02e8   /* COM1 */

#define BIT_0 0x01
#define BIT_1 0x02
#define BIT_2 0x04
#define BIT_3 0x08
#define BIT_4 0x10
#define BIT_5 0x20
#define BIT_6 0x40
#define BIT_7 0x80

/*  FIFO control bits*/

#define TRIGGER_LEVEL_16_1_BYTE	0x00
#define TRIGGER_LEVEL_4_BYTES BIT_6
#define TRIGGER_LEVEL_8_BYTES BIT_7
#define TRIGGER_LEVEL_14_BYTES (BIT_6|BIT_7)
#define TRIGGER_LEVEL_64_1_BYTE	0x00
#define TRIGGER_LEVEL_16_BYTES BIT_6
#define TRIGGER_LEVEL_32_BYTES BIT_7
#define TRIGGER_LEVEL_56_BYTES (BIT_6|BIT_7)
#define ENABLE_64_BIT_FIFO BIT_5
#define DMA_MODE_SELECT BIT_3
#define CLEAR_XMIT_FIFO BIT_2
#define CLEAR_RCV_FIFO BIT_1
#define ENABLE_FIFOS BIT_0


/* modem control bits */

#define ENABLE_AUTO_FLOW 	BIT_5
#define LOOPBACK_MODE 	BIT_4
#define AUX_OUT_2 		BIT_3
#define AUX_OUT_1 		BIT_2
#define RTS				BIT_1
#define DTR				BIT_0

/* modem status (MSR) bits */

#define CARRIER_DETECT 			BIT_7
#define RING_INDICATOR 				BIT_6
#define DSR 						BIT_5
#define CTS 						BIT_4
#define DELTA_DATA_CARRIER_DETECT 	BIT_3 
#define TRAILING_EDGE_RING 			BIT_2
#define DELTA_DSR 					BIT_1
#define DELTA_CTS 					BIT_0

#define ENABLE_DLAB 0x80
#define DISABLE_DLAB 0x00

#define ENABLE_INTERRUPTS 0x01
#define DISABLE_INTERRUPTS 0x00

#define DATA_REG 0
#define INT_ENABLE 1
#define DIV_LOBYTE 0
#define DIV_HIBYTE 1
#define INT_ID 2
#define FIFO_CTL 2
#define LINE_CTL 3
#define MODEM_CTL 4
#define LINE_STATUS 5
#define MODEM_STATUS 6
#define SCRATCH_REG 7


/* line status register bits */

#define DATA_READY 		0x01
#define OVERRUN_ERROR 	0x02
#define PARITY_ERROR 		0x04
#define FRAMING_ERROR 	0x08
#define BREAK_INDICATOR 	0x10
#define XMIT_HOLD_EMPTY 	0x20
#define TRANS_EMPTY 		0x40
#define IMPENDING_ERROR 	0x80

#define LOBYTE(b) ((u8int)(b&0x00FF))
#define HIBYTE(b) ((u8int)((b&0xFF00) >> 8))

#define CAT(a,b) a##b

#ifdef __x86_64__

#define OUTB(a,b) outb64(a,b)
#define OUTW(a,b) outw64(a,b)

#else

#define OUTB(a,b) outb(a,b)
#define OUTW(a,b) outw(a,b)

#endif

static u16int select_speed(u32int speed)
{
	switch(speed)
	{
		case 50:
			return 2304;
		case 300:
			return 384;
		case 600:
			return 192;
		case 2400:
			return 48;
		case 4800:
			return 24;
		case 9600:
			return 12;
		case 19200:
			return 6;
		case 38400:
			return 3;
		case 57600:
			return 2;
		case 115200:
			return 1;
		default:
			return 0;
	}
}

static u8int encode_bits( u8int bits )
{
	if( bits < 5 || bits > 8 )
		return 0;
	return (bits - 5);
}

static u8int encode_parity(u8int parity)
{
	switch(parity)
	{
		case PARITY_NONE:
			return ( 0x00 << 3);
		case PARITY_ODD:
			return ( 0x01 << 3 );
		case PARITY_EVEN:
			return ( 0x03 << 3);
		case PARITY_MARK:
			return ( 0x05 << 3 );
		case PARITY_SPACE:	
			return ( 0x07 << 3 );
		default:
			break;
	}
	return 0xFF;
}

static u16int encode_port(u16int port)
{
	switch(port)
	{
		case 1:
			return COM1;
		case 2:
			return COM2;
		case 3:
			return COM3;
		case 4:
			return COM4;
		default:
			break;
	}
	return 0;
}

static
inline
u8int init_serial_imp( u16int port, u32int speed, u8int bits, u8int parity, u8int stop)
{
	u8int fifo_ctl, modem_ctl;
	u16int divisor = select_speed(speed);
	if( divisor == 0 ) { return ERROR_BAD_SPEED; }
	
	bits = encode_bits(bits);
	if(bits == 0) { return ERROR_BAD_BITS; }
	
	parity = encode_parity(parity);
	if( parity == 0xFF ) {return ERROR_BAD_PARITY; }
	
	u8int flags = bits | parity;
	if(stop)
	{
		flags |= BIT_2;
	}
	port = encode_port(port);
	if( 0 == port ) { return ERROR_BAD_PORT; }
	
	// Enable FIFO, clear them, with 14-byte threshold
	fifo_ctl = (TRIGGER_LEVEL_14_BYTES|CLEAR_XMIT_FIFO|CLEAR_RCV_FIFO|ENABLE_FIFOS);
	
	// IRQs enabled, RTS/DSR set 
	// most UARTs need Auxiliary Output 2 set to a logical "1" to enable interrupts.
	modem_ctl = (AUX_OUT_2|RTS|DTR);
	
	OUTB( port + INT_ENABLE, DISABLE_INTERRUPTS );   // Disable all interrupts
	OUTB( port + LINE_CTL, ENABLE_DLAB);			// Enable DLAB (set baud rate divisor)
	OUTB( port + DIV_LOBYTE, LOBYTE(divisor) );		// Set divisor low byte
	OUTB( port + DIV_HIBYTE, HIBYTE(divisor) );		// Set divisor high byte
	OUTB( port + LINE_CTL, flags );					// set bits, parity, & stop bit options
	OUTB( port + FIFO_CTL, fifo_ctl ); 				// Enable FIFO, clear them, with 14-byte threshold 
	OUTB( port + MODEM_CTL, modem_ctl);				// IRQs enabled, RTS/DSR set
	return SUCCESS;
}

u8int SYM6432(init_serial)( u16int port, u32int speed, u8int bits, u8int parity, u8int stop)
{
	return init_serial_imp(port, speed, bits, parity, stop);
}

extern int printf(const char *fmt, ... );

static
inline
void identify_uart_imp(u16int port)
{
	port = encode_port(port);
	OUTB( port + FIFO_CTL, 0xE7);
	u8int flags = inb( port + INT_ID );
	if( 0 != (flags & 0x40) )
	{
		if( 0 != (flags & 0x80) )
		{
			if( 0 != (flags & 0x20) )
			{
				printf("UART 16750\n");
			}
			else
			{
				printf("UART 16550A\n");
			}
		}
		else
		{
			printf("UART 16550\n");
		}
	}
	else
	{
		OUTB( port + SCRATCH_REG, 0x2A );
		if( inb(port + SCRATCH_REG) == 0x2A )
		{
			printf("UART 16450\n");
		}
		else
		{
			printf("UART 8250\n");
		}
	}
}

void  SYM6432(identify_uart)(u16int port)
{
	identify_uart_imp(port);
}

static
int serial_received(u8int port) {
	return (inb(port +LINE_STATUS) & DATA_READY);
}

 static
char read_serial(u8int port) {
   while (serial_received(port) == 0);
 
   return inb(port);
}

static
int is_transmit_empty(u8int port) {
   return (inb(port + LINE_STATUS) & XMIT_HOLD_EMPTY);
}

static
inline
u8int serial_getc_imp( u16int port, char *c )
{
	int i;	
	if( NULL == c )
		return ERROR_NULL_POINTER;
	port = encode_port(port);
	if( port == 0 )
		return ERROR_BAD_PORT;
	for( i=0; i<50000; ++i )
	{
		if( serial_received(port) )
		{
			*c = inb(port);
			return SUCCESS;
		}
	}
	return ERROR_TIMEOUT;	
}

u8int SYM6432(serial_getc)( u16int port, char *c )
{
	return serial_getc_imp(port, c);
}


static
inline
u8int serial_putc_imp(u16int port, char c)
{
	int i;
	port = encode_port(port);
	if( port == 0 )
		return ERROR_BAD_PORT;
	for( i=0; i<50000; ++i )
	{
		if( is_transmit_empty(port) )
		{
			OUTB(port,c);
			return SUCCESS;
		}
	}
	return ERROR_TIMEOUT;
}

u8int SYM6432(serial_putc)(u16int port, char c)
{
	return serial_putc_imp(port, c);
}

static
inline
u8int serial_write_imp(u16int port, const char *str)
{
	while( *str )
	{
		u8int status = serial_putc_imp(port, *str);
		if( status != SUCCESS )
		{
			return status;
		}
		++str;
	}
	return SUCCESS;	
}

u8int SYM6432(serial_write)(u16int port, const char *str)
{
	return serial_write_imp(port, str);
}


void SYM6432(write_serial)(char a) {
    serial_putc_imp(1, a);
}

static
inline
void write_string_imp(const char *str)
{
	while(*str)
	{
		serial_putc_imp(1, *str);
		++str;
	}	
}

void SYM6432(write_string)(const char *str)
{
	write_string_imp(str);
}
