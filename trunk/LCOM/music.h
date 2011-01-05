#ifndef _MUSIC_H_
#define _MUSIC_H_

#include "timer.h"


/** @defgroup Music Music
 * @{
 *
 * Music related functions
 */

/** Note definition
 */
typedef struct {
	int freq; 	/**< note frequency */
	int dur;	/**< note duration in miliseconds */
} Note;


/** Turns the speaker on
 */
void speaker_on();


/** Turns the speaker off
 */
void speaker_off();


/** Plays a single note
 */
void play_note(Note *note);


/** @} end of music */

#endif
