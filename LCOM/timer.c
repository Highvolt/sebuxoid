#include "timer.h"
/*
void c_wait(void)
{
	msec++;
	//printf("contador: %i\n", ms);
	outportb(PIC1_CMD, EOI);
}
*/

/** Program timer (0,1,2) with mode
 */
void timer_init(int timer, int mode)
{
	outportb(TIMER_CTRL, mode | (timer << 6));
	
	if(timer == 2)
	{
		char val = inportb(SPEAKER_CTRL);
		outportb(SPEAKER_CTRL, val | TIMER2_ENABLE);
	}
}


/** Load timer (0,1,2) with value
 */
void timer_load(int timer, int value)
{
	outportb(0x40 | timer, LSB(value));
	outportb(0x40 | timer, MSB(value));
}


//Obter divisor para a frequencia 'freq'
int get_divider(int freq)
{
	return (TIMER_CLK / freq);
}


/** Wait mili miliseconds, indirectly counting T0 interrupts
*/
void mili_sleep(int mili)
{
	int timeout = msec + mili;
	while(msec < timeout) HLT;
}


/** wait secs seconds, indirectly counting T0 interrupts
 */

void sec_sleep(int secs)
{
	mili_sleep(secs * 1000);
}

