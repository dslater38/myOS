#include "rtc.h"
#include <stdint.h>
#include "isr.h"
#include <ctime>

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

constexpr uint8_t RTC_REGISTER_SELECT_PORT = 0x70u;
constexpr uint8_t RTC_REGISTER_DATA_PORT = 0x71u;

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


    printf("%02.2u/%02.2u/%04.4u %02.2u:%02.2u:%02.2u\n",month, day, (cen*100u + year), hr, min, sec);

    return 0;
}