#ifndef __ATA_H_
#define __ATA_H_

#include <stdint.h>
// #include "common.h"

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170
#define	ATA_TERTIARY_IO  0x1E8
#define	ATA_QUATERNARY_IO  0x168

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376
#define ATA_TERTIARY_DCR_AS 0x3E6
#define ATA_QUATERNARY_DCR_AS 0x366


#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

#if 0
#define ATA_PORTS_BASE 0x01F0
#define ATA_DATA_REG (ATA_PORTS_BASE + 0)		// Data Register Read/Write PIO data bytes (R/W) (16-bit/16-bit)
#define ATA_ERROR_REG (ATA_PORTS_BASE + 1)		// Error register Used to retrieve any error generated by the last ATA command executed (R) (8-bit/16-bit)
#define ATA_FEATURES_REG (ATA_PORTS_BASE + 1)	// Features Register Used to control command specific interface features. (W) (8-bit/16-bit)
#define ATA_SEC_COUNT_REG (ATA_PORTS_BASE + 2)	// Sector Count Register Number of sectors to read/write (0 is a special value). (R/W) (8-bit/16-bit)
#define ATA_SEC_NUM_REG (ATA_PORTS_BASE + 3)	// Sector Number Register (LBAlo) This is CHS / LBA28 / LBA48 specific. (R/W) (8-bit/16-bit)
#define ATA_CYL_LOW_REG (ATA_PORTS_BASE + 4)	// Cylinder Low Register / (LBAmid) Partial Disk Sector address. (R/W) (8-bit/16-bit)
#define ATA_CYL_HIGH_REG (ATA_PORTS_BASE + 5)	// Cylinder High Register / (LBAhi) Partial Disk Sector address. (R/W) (8-bit/16-bit)
#define ATA_DRIVE_HEAD_REG (ATA_PORTS_BASE + 6)	// Drive / Head Register Used to select a drive and/or head. Supports extra address/flag bits.(R/W) (8-bit/16-bit)
#define ATA_STATUS_REG (ATA_PORTS_BASE + 7)		// Status Register Used to read the current status. (R) (8-bit/8-bit)
#define ATA_COMMAND_REG (ATA_PORTS_BASE + 7)	// Command Register Used to send ATA commands to the device. (W) (8-bit/8-bit)

#define ATA_CONTROL_BASE 0x03F6
#define ATA_ALTERNATE_STATUS_REG (ATA_CONTROL_BASE + 0)	// Alternate Status Register A duplicate of the Status Register which does not affect interrupts. (R) (8-bit/8-bit)
#define ATA_DEVICE_CONTROL_REG   (ATA_CONTROL_BASE + 0)	// Device Control Register	Used to reset the bus or enable/disable interrupts. (W) (8-bit/8-bit)
#define ATA_DRIVE_ADDRESS_REG	 (ATA_CONTROL_BASE + 1)	// Drive Address Register	Provides drive select and head select information. (R) (8-bit/8-bit)
#endif

#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define      ATAPI_CMD_READ       0xA8
#define      ATAPI_CMD_EJECT      0x1B

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01
 
#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

#define ATA_REG_CYL_LO	0x04
#define ATA_REG_CYL_HI	0x05


#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

// #define ATA_REG_ALTSTATUS  0x00
// #define ATA_REG_DEVADDRESS 0x01

// Channels:
#define      ATA_PRIMARY      0x00
#define      ATA_SECONDARY    0x01
 
// Directions:
#define      ATA_READ      0x00
#define      ATA_WRITE     0x013

#if 0

typedef struct {
	uint8_t drive;
} ide_private_data;



extern void ata_init();
void detectControllers();
void write(void *buffer, uint32_t address, size_t size);
void read(void *buffer, uint32_t address, size_t size);
#endif // 0

#endif
