#include "rtc.h"





void rtc_int_enable()
{
	write_rtc(RTC_STAT_B, read_rtc(RTC_STAT_B) | RTC_PIE | RTC_AIE | RTC_UIE);
	read_rtc(RTC_STAT_C);
}





void rtc_int_disable()
{
	write_rtc(RTC_STAT_B, read_rtc(RTC_STAT_B) & ~(RTC_PIE | RTC_AIE | RTC_UIE));
}



/** Returns rtc data from I/O address add. Doesn't check valid bit
 */
Byte read_rtc(Byte add)
{
	int old_pic1 = get_pic1(), old_pic2 = get_pic2();
	set_pic_var(0xFF, 0xFF);

	outportb(RTC_ADDR_REG, add);
	Byte val = inportb(RTC_DATA_REG);

	set_pic_var(old_pic1, old_pic2);
	return val;
}




/** Write value to I/O address add. Doesn't check valid bit
 */
void write_rtc(Byte add, Byte value)
{
	int old_pic1 = get_pic1(), old_pic2 = get_pic2();
	set_pic_var(0xFF, 0xFF);

	outportb(RTC_ADDR_REG, add);
	outportb(RTC_DATA_REG, value);

	set_pic_var(old_pic1, old_pic2);
}

