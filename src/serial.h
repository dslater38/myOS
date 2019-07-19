#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "sym6432.h"


uint8_t SYM6432(init_serial)( uint16_t port, uint32_t speed, uint8_t bits, uint8_t parity, uint8_t stop);
uint8_t SYM6432(serial_putc)(uint16_t port, char c);
uint8_t SYM6432(serial_write)(uint16_t port, const char *str);
uint8_t SYM6432(serial_getc)( uint16_t port, char *c );
void  SYM6432(identify_uart)(uint16_t port);

/* valid baud rates */
#define BAUD_50 50
#define BAUD_300 300
#define BAUD_600 600
#define BAUD_2400 2400
#define BAUD_4800 4800
#define BAUD_9600 9600
#define BAUD_19200 119200
#define BAUD_38400 38400
#define BAUD_115200 115200

/* valid bits settings */
#define BITS_5 5
#define BITS_6 6
#define BITS_7 7
#define BITS_8 8

/* valid parity settings */

#define PARITY_NONE 0
#define PARITY_ODD 1
#define PARITY_EVEN 2
#define PARITY_MARK 3
#define PARITY_SPACE 4

/* valid stop bits */
#define ONE_STOP_BIT 0x02
#define NO_STOP_BITS 0x00

/* success code */
#define SUCCESS 0

/* error codes */
#define ERROR_BAD_SPEED 1
#define ERROR_BAD_BITS 2
#define ERROR_BAD_PARITY 3
#define ERROR_BAD_PORT 4
#define ERROR_TIMEOUT 5
#define ERROR_NULL_POINTER 6
#define ERROR_PORT_NOT_READY 7

#undef IMPORT
#undef CAT

#ifdef __cplusplus
}
#endif
#endif //SERIAL_H_INCLUDED
