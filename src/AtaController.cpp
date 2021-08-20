#include "AtaController.h"
#include "common.h"
#include "ata.h"
#include <array>

enum class IoDirection {
    Read,
    Write
};


template<uint16_t REGS_BASE, uint16_t CTRL_BASE>
class AtaControllerT : public AtaController
{
public:
    AtaControllerT()=default;
    size_t write(Drive drive, void *buffer, uint32_t address, size_t size)const override
    {
        size_t written{0};
        const auto *ptr = reinterpret_cast<const uint16_t *>(buffer);
        auto numSectors = (size>>9);  // 512 byte sectors

        while( numSectors > 0)
        {
            const auto writeCount = std::min(numSectors, 256ul );
            if(write_sectors(drive, ptr, static_cast<uint8_t>(writeCount), address))
            {
                ptr += 256*writeCount;     // we just wrote 256 words numSectors times for 256*numSectors words total.
                address += 512*writeCount; 
                size -= 512*writeCount;
                written += 512*writeCount;
                numSectors -= writeCount;
            }
            else
            {
                debug_out("write_sectors() failed.\n");
                return 0;
            }
        }

        if(size>0)
        {
            std::array<uint16_t,256> tmp{};
            if( read_sectors(drive, tmp.data(), 1, address) )
            {
                memcpy(tmp.data(), ptr, size);
                if(write_sectors(drive, tmp.data(), 1, address))
                {
                    written += size;
                }
            }
        }
        if(written > 0 )
        {
            cacheFlush();
        }
        return written;
    }
    
    size_t read(Drive drive, void *buffer, uint32_t address, size_t size)const override
    {
        size_t read{0};
        auto *ptr = reinterpret_cast<uint16_t *>(buffer);
        auto numSectors = (size>>9);  // 512 byte sectors

        while( numSectors > 0)
        {
            const auto readCount = std::min(numSectors, 256ul );
            if(read_sectors(drive, ptr, static_cast<uint8_t>(readCount), address))
            {
                ptr += 256*readCount;     // we just read 256 words numSectors times for 256*numSectors words total.
                address += 512*readCount; 
                size -= 512*readCount;
                read += 512*readCount;
                numSectors -= readCount;
            }
            else
            {
                debug_out("read_sectors() failed.\n");
                return 0;
            }
        }

        if(size>0)
        {
            std::array<uint16_t,512> tmp{};
            if( read_sectors(drive, tmp.data(), 1, address) )
            {
                memcpy(ptr, tmp.data(), size);        
                read += size;        
            }
        }
        return read;
    }
    AtaDeviceType detect(Drive drv)const override
    {
        soft_reset();
        selectDriveCHS(drv, 0);
        io_wait();

        const auto c = readWord(REGS_BASE+ATA_REG_CYL_LO, REGS_BASE+ATA_REG_CYL_HI);
        switch(c)
        {
            case 0xEB14:
                return AtaDeviceType::PATAPI;
            case 0x9669:
                return AtaDeviceType::SATAPI;
            case 0x0000:
                return AtaDeviceType::PATA;
            case 0xC33C:
                return AtaDeviceType::SATA;
            default:
                break;
        }

        return AtaDeviceType::UNKNOWN;
    }
private:
    static void cacheFlush()
    {
        outb(REGS_BASE+ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    }

    static bool read_sectors(Drive drive, uint16_t *buffer, uint8_t count, uint32_t address)
    {
        setup_for_io(drive, address, count, IoDirection::Read);
        size_t num = count ? count : 256;   // a count of 0 == 256 sectors

        for(auto i=0u; i<num; ++i )
        {
            ide_poll();
            insw(REGS_BASE + ATA_REG_DATA, buffer, 256);
            buffer += 256;
        }
        return true;
    }
    
    static bool write_sectors(Drive drive, const uint16_t *buffer, size_t count, uint32_t address)
    {
        setup_for_io(drive, address, count, IoDirection::Write);
        auto *ptr = buffer;
        size_t num = count ? count : 256;   // a count of 0 == 256 sectors

        for(auto i=0u; i<num; ++i )
        {
            // can't use rep outsw for writing
            // There needs to be a small delay between each call to outw
            ide_poll();
            for( auto j=0u; j<256u; ++j )
            {                
                outw(REGS_BASE + ATA_REG_DATA, *ptr);
                ++ptr;
            }
        }
        return true;
    }
    bool identify(Drive drv)const override
    {
        static uint16_t buffer[256];
        const uint8_t drvSel = drv==Drive::Primary ? 0xA0 : 0xB0;
        const auto sDrv =  drv==Drive::Primary ? "Primary": "Secondary";
        outb(REGS_BASE + ATA_REG_HDDEVSEL, drvSel);
        outb(REGS_BASE + ATA_REG_SECCOUNT0, 0x00);
        outb(REGS_BASE + ATA_REG_LBA0, 0x00);
        outb(REGS_BASE + ATA_REG_LBA1, 0x00);
        outb(REGS_BASE + ATA_REG_LBA2, 0x00);
        outb(REGS_BASE + ATA_REG_COMMAND, 0xEC); // IDENTIFY command
        auto status = inb(REGS_BASE + ATA_REG_STATUS);
        if(status == 0x00)
        {
            printf("%s drive doesn't exist\n", sDrv);
            return false;
        }
        status = 0;
        do {
            status = readStatusReg();
        }while( (status & ATA_SR_BSY) ==  ATA_SR_BSY );
        auto ck = inb(REGS_BASE + ATA_REG_LBA1);
        if(ck != 0x00)
        {
            printf("%s drive is not an ATA drive. Aborting\n", sDrv);
            return false;
        }
        ck = inb(REGS_BASE + ATA_REG_LBA2);
        if(ck != 0x00)
        {
            printf("%s drive is not an ATA drive. Aborting\n", sDrv);
            return false;
        }
        do {
            status = readStatusReg();
        }while((status & (ATA_SR_DRQ|ATA_SR_ERR)) == 0x00 );
        if( (status & ATA_SR_ERR) == ATA_SR_ERR )
        {
            printf("%s drive: readStatusReg() returned ATA_SR_ERR. Aborting\n", sDrv);
            return false;
        }
        insw(REGS_BASE + ATA_REG_DATA, buffer, 256);
        uint16_t b83 = buffer[83];
        if( (b83 & (1<<10)) != 0 )
        {
            printf("%s drive supports LBA48.\n", sDrv);
        }
        else
        {
            printf("%s drive does NOT support LBA48.\n", sDrv);
        }
        uint32_t totalLBA28Sectors = *reinterpret_cast<uint32_t *>(buffer + 60);
        if(totalLBA28Sectors > 0)
        {
            printf("%s drive has %u total LBA28 addressable sectors.\n", sDrv, totalLBA28Sectors);

        }
        uint64_t totalLBA48Sectors = *reinterpret_cast<uint64_t *>(buffer + 100);
        if(totalLBA48Sectors > 0)
        {
            printf("%s drive has %lu total LBA48 addressable sectors.\n", sDrv, totalLBA48Sectors);
        }
        return true;
    }

private:
    static void io_wait()
    {
        readAltStatusReg();
        readAltStatusReg();
        readAltStatusReg();
        readAltStatusReg();
    }

    static void soft_reset()
    {
        outb(CTRL_BASE, 0x04);
        io_wait();
        outb(CTRL_BASE, 0x00);
    }

    // static void ide_wait_irq() {
    // while (!channels[channel].interrupt)
    //     asm("hlt");
    // channels[channel].interrupt = 0;
    // }

    static void ide_poll()
    {
        //ide_wait_irq(ATA_PRIMARY);
        // return ;
        for(int i=0; i< 4; i++)
        {
            ide_read(ATA_REG_ALTSTATUS);
        }
    //	for(int i=0; i< 4; i++)
    //		inb(io + ATA_REG_ALTSTATUS);

    retry:;
        // uint8_t status = inb(io + ATA_REG_STATUS);
        uint8_t status = ide_read(ATA_REG_ALTSTATUS);
        //mprint("testing for BSY\ATA_PRIMARY, 
        if(status & ATA_SR_BSY) goto retry;
        //mprint("BSY cleared\n");
    retry2:	status = inb(REGS_BASE + ATA_REG_STATUS);
        if(status & ATA_SR_ERR)
        {
            PANIC("ERR set, device failure!\n");
        }
        //mprint("testing for DRQ\n");
        if(!(status & ATA_SR_DRQ)) goto retry2;
        //mprint("DRQ set, ready to PIO!\n");
        return;
    }


    // static void ide_poll()
    // {
    //     io_wait();
    //     uint8_t status{0};
    //     auto count = 0;
    //     do {
    //         status = readStatusReg();
    //         if(++count > 1000000)
    //         {
    //             printf("ide_poll: timeout waiting for ATA_SR_BSY to clear.");
    //             return;
    //         }
    //     }while( (status & ATA_SR_BSY) ==  ATA_SR_BSY );
    //     count = 0;
    //     do{
    //         status = readStatusReg();
    //         if(status & ATA_SR_ERR)
    //         {
    //             PANIC("ERR set, device failure!\n");
    //         }
    //         if(++count > 1000000)
    //         {
    //             printf("ide_poll: timeout waiting for ATA_SR_DRQ to set.");
    //             return;
    //         }
    //     }while((status & ATA_SR_DRQ) == 0);
    // }

    static constexpr uint32_t make_regio(uint16_t reg, uint8_t data)
    {
        return ((static_cast<uint32_t>(reg)<<16)|static_cast<uint32_t>(data));
    }

    static uint16_t select_port(uint8_t reg)
    {
        return (reg < 0x08) ? REGS_BASE  + reg - 0x00 :
            (reg < 0x0C) ? REGS_BASE  + reg - 0x06 :
            (reg < 0x0E) ? CTRL_BASE  + reg - 0x0A :
            (reg < 0x16) ? 0 + reg - 0x0E : 0;

    }

    static void ide_write(uint8_t reg, uint8_t data) {
        if (reg > 0x07 && reg < 0x0C)
            ide_write(ATA_REG_CONTROL, 0x80 | 0);

        const auto port = select_port(reg);
        if(port > 0 )
        {
            outb(port, data);
        }
        
        if (reg > 0x07 && reg < 0x0C)
            ide_write(ATA_REG_CONTROL, 0);
    }

    static uint8_t ide_read(uint8_t reg) {
        uint8_t result;
        if (reg > 0x07 && reg < 0x0C)
            ide_write(ATA_REG_CONTROL, 0x80 | 0);

        const auto port = select_port(reg);
        if(port > 0 )
        {
            result = inb(port);
        }

        if (reg > 0x07 && reg < 0x0C)
            ide_write(ATA_REG_CONTROL, 0);
        return result;
    }


    static void setup_for_io(Drive drive, uint32_t address, uint8_t count, IoDirection dir)
    {
#if 0        
        const uint8_t drv = drive==Drive::Primary ? 0x00 : 0x10;
        uint8_t cmd = ((dir == IoDirection::Write) ? ATA_CMD_WRITE_PIO : ATA_CMD_READ_PIO);

        uint32_t io[] = {
            make_regio( REGS_BASE + ATA_REG_HDDEVSEL,(uint8_t)( (0xE0|drv) | (uint8_t)(( (address >> 24) & 0x0F))) ),
            make_regio( REGS_BASE + 1,0x00 ),
            make_regio( REGS_BASE + ATA_REG_SECCOUNT0,count ),
            make_regio( REGS_BASE + ATA_REG_LBA0,(uint8_t)(address) ),
            make_regio( REGS_BASE + ATA_REG_LBA1,(uint8_t)(address >> 8) ),
            make_regio( REGS_BASE + ATA_REG_LBA2,(uint8_t)(address >> 16) ),
            make_regio( REGS_BASE + ATA_REG_COMMAND,cmd )
        };

        uint16_t regs[7]={
            REGS_BASE + ATA_REG_HDDEVSEL,
            REGS_BASE + 1,
            REGS_BASE + ATA_REG_SECCOUNT0,
            REGS_BASE + ATA_REG_LBA0,
            REGS_BASE + ATA_REG_LBA1,
            REGS_BASE + ATA_REG_LBA2,
            REGS_BASE + ATA_REG_COMMAND
        };

        uint8_t data[7] = {
            (uint8_t)( (0xE0|drv) | (uint8_t)(( (address >> 24) & 0x0F))),
            0x00,
            0x01,
            (uint8_t)(address),
            (uint8_t)(address >> 8),
            (uint8_t)(address >> 16),
            cmd
        };

        // batch_outb(regs, data, 7);
	    batch_outb2(io, 7);
        // ide_poll();
#else
	    uint8_t cmd = ((dir  == IoDirection::Write) ? ATA_CMD_WRITE_PIO : ATA_CMD_READ_PIO);

        ide_write(ATA_REG_HDDEVSEL, (0xE0 | (uint8_t)((address >> 24 & 0x0F))));        // Select the drive
       
        ide_write(ATA_REG_FEATURES, 0x00);                                                                  // set features flags 

        ide_write(ATA_REG_SECCOUNT0, count);                                                            // set the number of sectors to read/write

        auto c = static_cast<uint8_t>(address & 0x000000FF);
        ide_write(ATA_REG_LBA0, c);                                                                             // set LBA28 address lowest 8 bits

        c = static_cast<uint8_t>((address >> 8) & 0x000000FF);
        ide_write(ATA_REG_LBA1, c);                                                                             // set LBA28 address middle 8bits

        c = static_cast<uint8_t>((address >> 16) & 0x000000FF);
        ide_write(ATA_REG_LBA2, c);                                                                             // set LBA28 address highest 8 bits

        ide_write(ATA_REG_COMMAND, cmd);                                                                // send the read/write command
        
        // ide_poll();
#endif // 0        
    }
private:
    static uint8_t readStatusReg()
    {
        return inb(REGS_BASE + ATA_REG_STATUS);
    }

    static uint8_t readAltStatusReg()
    {
        return inb(CTRL_BASE + ATA_REG_ALTSTATUS);
    }

    static uint16_t readWord(uint16_t portLo, uint16_t portHi)
    {
        uint8_t cl = inb(portLo);
        uint8_t ch = inb(portHi);
        return ((static_cast<uint16_t>(ch)<<8) | static_cast<uint16_t>(cl));
    }

    static void selectDriveCHS(Drive drv, uint8_t head)
    {
        const uint8_t cmd = (drv==Drive::Primary ? 0xA0 : 0xB0);
        outb(REGS_BASE+ATA_REG_HDDEVSEL, (cmd | head) );
    }
};

const AtaController & AtaController::primary()
{
    static AtaControllerT<ATA_PRIMARY_IO, ATA_PRIMARY_DCR_AS> controller{};
    return controller;
}

const AtaController & AtaController::secondary()
{
    static AtaControllerT<ATA_SECONDARY_IO, ATA_SECONDARY_DCR_AS> controller{};
    return controller;
}
