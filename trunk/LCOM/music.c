#include "music.h"


	

/** Turns the speeker on
 */
void speaker_on()
{
	char val = inportb(SPEAKER_CTRL);
	outportb(SPEAKER_CTRL, val | SPEAKER_ENABLE);
}


/** Turns the speeker off
 */
void speaker_off()
{
	char val = inportb(SPEAKER_CTRL);
	outportb(SPEAKER_CTRL, val & ~SPEAKER_ENABLE); //val & 0xFE
}


/** Plays a single note
 */
void play_note(Note *note)
{
	// if(note->freq == PAUSE_FREQ)
		// mili_sleep(note->dur);
	// else
	// {
		timer_load(TIMER_2, get_divider(note->freq));

		// speaker_on();
		// mili_sleep(note->dur);
		// speaker_off();
	// }
}
