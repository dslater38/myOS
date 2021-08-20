#include "rtc.h"
#include <stdint.h>
#include "isr.h"
#include <ctime>
#include "TextFrameBuffer.h"

constexpr uint16_t	RTC_SECONDS = 0x00;
constexpr uint16_t	RTC_MINUTES = 0x02;
constexpr uint16_t	RTC_HOURS = 0x04;

constexpr uint16_t	RTC_WEEKDAY = 0x06;
constexpr uint16_t	RTC_DAY = 0x07;
constexpr uint16_t	RTC_MONTH = 0x08;
constexpr uint16_t	RTC_YEAR = 0x09;
constexpr uint16_t	RTC_CENTURY = 0x32;
constexpr uint16_t	RTC_REG_A = 0x0A;
constexpr uint16_t	RTC_REG_B = 0x0B;
constexpr uint16_t	RTC_REG_C = 0x0C;
constexpr uint16_t	RTC_REG_D = 0x0D;


constexpr uint16_t	RTC_FREQ_SELECT = RTC_REG_A;


constexpr uint16_t	RTC_PORT = 0x71;

constexpr uint16_t	CMOS_SELECT_REG = 0x70;

static void enable_nmi()
{
    outb(0x70, inb(0x70) & 0x7F);
}

static void disable_nmi()
{
    outb(0x70, inb(0x70) | 0x80 );
}

void RTC_update(registers64 regs)
{
    disable_nmi();
}

constexpr uint8_t CURRENT_SECONDS   = 0x00u;
constexpr uint8_t CURRENT_MINUTES   = 0x02u;
constexpr uint8_t CURRENT_HOURS     = 0x04u;
constexpr uint8_t CURRENT_DATE      = 0x07u;
constexpr uint8_t CURRENT_MONTH     = 0x08u;
constexpr uint8_t CURRENT_YEAR      = 0x09u;
constexpr uint8_t CURRENT_CENTURY   = 0x32u;

constexpr uint8_t STATUS_REGISTER_A = 0x0Au;
constexpr uint8_t STATUS_REGISTER_B = 0x0Bu;
constexpr uint8_t STATUS_REGISTER_C = 0x0Cu;
constexpr uint8_t STATUS_REGISTER_D = 0x0Du;

constexpr uint8_t DISABLE_NMI_BIT   = 0x80u;

constexpr uint8_t RTC_REGISTER_SELECT_PORT = 0x70u;
constexpr uint8_t RTC_REGISTER_DATA_PORT = 0x71u;

// Status Register A bits - Read/Write except for RTC_UIP_BIT
constexpr uint8_t RTC_UIP_BIT           = 0x80u;    // Register A update in progress bit
constexpr uint8_t RTC_DIV2_BIT          = 0x40u;    // divider bits that define RTC operating frequency
constexpr uint8_t RTC_DIV1_BIT          = 0x20u;    // should be 010b
constexpr uint8_t RTC_DIV0_BIT          = 0x10u;    //
constexpr uint8_t RTC_RS3_BIT           = 0x08u;    // Rate selection bits that define the periodic
constexpr uint8_t RTC_RS2_BIT           = 0x04u;    // interrupt rate (default 0110b)
constexpr uint8_t RTC_RS1_BIT           = 0x02u;
constexpr uint8_t RTC_RS0_BIT           = 0x01u;

// Status Register B bits  - Read/Write
constexpr uint8_t RTC_SET_BIT           = 0x80u;    // SET bit - set to 1 to abort any update that's in progress
constexpr uint8_t RTC_PIE_BIT           = 0x40u;    // PIE (Periodic Interrupt Enable) when set the periodic interrupt will occur at the frequency specified by RS bits in Status Register A.
constexpr uint8_t RTC_AIE_BIT           = 0x20u;    // (AIE) Alarm Interrupt Enable, when set the alarm interrupt will be asserted once for each second that the current time matches the alarm time.
constexpr uint8_t RTC_UIE_BIT           = 0x10u;    // (UIE) Update-ended Interrupt Enable, when set the update-ended interrupt will be asserted once each second after the end of update cycle. This bit is cleared when SET bit goes high but it is not reset when SET is cleared.
constexpr uint8_t RTC_SQWE_BIT          = 0x08u;    // (SQWE) Square Wave Enable, when set, enables the square wave output on the SQW pin at the frequency specified by the RS bits in the Status Register A. The SQW pin is not connected to anything in the AT.
constexpr uint8_t RTC_DM_BIT            = 0x04u;    // (DM) Data Mode, indicates mode for clock/calendar data: 0=BCD and 1=binary, BIOS setting is 0.
constexpr uint8_t RTC_HRFMT_BIT         = 0x02u;    // (24/12) - controls hours byte, 0=12-hour and 1=24-hour format, BIOS setting is 1.
constexpr uint8_t RTC_DSE_BIT           = 0x01u;    // (DSE) Daylight Savings Enable, when set two special updates will occur: last Sunday in April time will go 01:59:59 > 03:00:00 and last Sunday in October 01:59:59 > 01:00:00. BIOS sets it to 0 (ie. no daylight saving).

// Status Register C bits (Read-only)
constexpr uint8_t RTC_IRQF_BIT          = 0x80u;    // (IRQF) Interrupt Request Flag, when set one of the interrupts enabled in Status Register B has occurred.
constexpr uint8_t RTC_PF_BIT            = 0x40u;    // (PF) Periodic interrupt Flag, when set the periodic interrupt has occurred.
constexpr uint8_t RTC_AF_BIT            = 0x20u;    // (AF) Alarm interrupt Flag, when set the alarm interrupt has occurred.
constexpr uint8_t RTC_UF_BIT            = 0x10u;    // (UF)  Update-ended interrupt Flag, when set the update-ended alarm interrupt has occurred.

// Status Register D - Read-only
constexpr uint8_t RTC_VRT_BIT           = 0x80u;    // (VRT)  Valid RAM and Time, OK when set, when clear indicates power was lost.


uint8_t
fromBDC(uint8_t bcd)
{
    return (((bcd & 0xF0)>>4)*10 + (bcd & 0x0F));
}

uint64_t
JulianDay(int64_t M, int64_t D, int64_t Y)
{
    return (1461 * (Y+4800+(M-14)/12)/4 + (367*(M-2-12*((M-14)/12)))/12 - (3*((Y+4900+(M-14)/12)/100))/4 + D - 32075);
}

uint64_t RTC_currentTime_nowait()
{
    outb(RTC_REGISTER_SELECT_PORT, CURRENT_SECONDS);
    auto sec = fromBDC(inb(RTC_REGISTER_DATA_PORT));

    outb(RTC_REGISTER_SELECT_PORT, CURRENT_MINUTES);
    auto min = fromBDC(inb(RTC_REGISTER_DATA_PORT));

    outb(RTC_REGISTER_SELECT_PORT, CURRENT_HOURS);
    auto hr = fromBDC(inb(RTC_REGISTER_DATA_PORT));

    outb(RTC_REGISTER_SELECT_PORT, CURRENT_DATE);
    auto day = fromBDC(inb(RTC_REGISTER_DATA_PORT));

    outb(RTC_REGISTER_SELECT_PORT, CURRENT_MONTH);
    auto month = fromBDC(inb(RTC_REGISTER_DATA_PORT));

    outb(RTC_REGISTER_SELECT_PORT, CURRENT_YEAR);
    auto year = fromBDC(inb(RTC_REGISTER_DATA_PORT));

    outb(RTC_REGISTER_SELECT_PORT, CURRENT_CENTURY);
    auto cen = fromBDC(inb(RTC_REGISTER_DATA_PORT));

    char buffer[24];
    sprintf(buffer,"%02.2u/%02.2u/%04.4u %02.2u:%02.2u:%02.2u",month, day, (cen*100u + year), hr, min, sec);

    auto *v = getVideo();

    if(v)
    {
        v->write_at(1, 60, TextColors::RED, TextColors::BLUE, buffer);
    }

    return 0;
}

uint64_t RTC_currentTime()
{
    for(;;)
    {
        outb(RTC_REGISTER_SELECT_PORT, STATUS_REGISTER_A);
        auto test = inb(RTC_REGISTER_DATA_PORT);
        if( (test & 0x80) == 0 )
        {
            break;
        }
    }
    return RTC_currentTime_nowait();
}

bool initial_update = false;

uint64_t    counter = 0;

void rtc_set_interrupt_rate(uint8_t rate)
{
    outb(RTC_REGISTER_SELECT_PORT, (DISABLE_NMI_BIT|STATUS_REGISTER_A));    // select status register A & disable NMI
    uint8_t prev = inb(RTC_REGISTER_DATA_PORT);                             // get initial value in a register
    outb(RTC_REGISTER_SELECT_PORT, (DISABLE_NMI_BIT|STATUS_REGISTER_A));    // select A again
    outb(RTC_REGISTER_DATA_PORT, (prev & 0xF0) | rate);                     // set the periodic interrupt rate
}

void rtc_interrupt_handler(registers64_t regs)
{
    outb(RTC_REGISTER_SELECT_PORT, STATUS_REGISTER_C);
    uint8_t stat = inb(RTC_REGISTER_DATA_PORT);

    // printf("rtc_interrupt_handler\n");

    if( stat & RTC_UF_BIT)
    {
        if(!initial_update)
        {
            initial_update = true;
            printf("RTC Current Time:\n");
           
            rtc_set_interrupt_rate(0x06u);
            outb(RTC_REGISTER_SELECT_PORT, (DISABLE_NMI_BIT|STATUS_REGISTER_B));    // select register B disable NMI
            uint8_t prev = inb(RTC_REGISTER_DATA_PORT);                             // fetch the current contents
            outb(RTC_REGISTER_SELECT_PORT, (DISABLE_NMI_BIT|STATUS_REGISTER_B));    // select register B again (a read will reset the index to register D)
            // outb(RTC_REGISTER_DATA_PORT, ( prev & (~RTC_UIE_BIT)) | RTC_PIE_BIT );
        }
        RTC_currentTime_nowait();
    }
    else if(stat & RTC_PF_BIT )
    {
        ++counter;
      //  printf("rtc: %ld\n",counter);
    }
}

void init_rct_interrupts()
{
    register_interrupt_handler64(IRQ8, rtc_interrupt_handler);
    constexpr uint8_t rate = 0x06u;                                         // rate is 64 == interrupt every 15.625 milliseconds
    asm("cli");      
    outb(RTC_REGISTER_SELECT_PORT, (DISABLE_NMI_BIT|STATUS_REGISTER_B));    // select register B disable NMI
    uint8_t prev = inb(RTC_REGISTER_DATA_PORT);                             // fetch the current contents
    outb(RTC_REGISTER_SELECT_PORT, (DISABLE_NMI_BIT|STATUS_REGISTER_B));    // select register B again (a read will reset the index to register D)
    outb(RTC_REGISTER_DATA_PORT, prev | RTC_UIE_BIT );                      // enable the periodic and update ended interrupts
    enable_nmi();                                                           // make sure NMI's are turned back on
    asm("sti");                                                             // re-enable interrupts
}