#include "rtc.h"
#include <stdint.h>
#include "isr.h"

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

void RTC_update(registers64 regs)
{
}