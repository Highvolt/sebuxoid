#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utypes.h"
#include "video-graphics.h"
#include "music.h"


/** @defgroup sprite Sprite
 * @{
 *
 * Sprite related functions
 */

/** A Sprite is an "object" that contains all needed information to
 * create, animate, and destroy a pixmap.  The functions assume that
 * the background is BLACK and they take into account collision with
 * other graphical objects or the screen limits. 
 */
typedef struct {
	int x, y;            ///< current sprite position
	int width, height;   ///< sprite dimensions
	int xspeed, yspeed;  ///< sprite current speed in the x and y direction
	char *map;           ///< the sprite pixmap (use read_xpm())
	char *bgmap;		   ///<the sprite background map
	Bool draw;
	Note * nota;
} Sprite;

#define MAX_SPEED 5    /**< each sprite maximum speed in pixels/frame */
#define RIGHT_HIT 2    /**< collision with right block (WHITE)  */
#define LEFT_HIT 3     /**< collision with left block (WHITE)   */
#define MOUSE_HIT 4    /**< collision with mouse (LIGHTMAGENTA) */

/** Reads a xpm-like sprite defined in "map" (look at pixmap.h for
 * examples). Returns the address of the allocated memory where the
 * sprite was read. Updates "width" and "heigh" with the sprite
 * dimension.  Return NULL on error.
 * Assumes that VRES and HRES (the screen vertical and horizontal resolution)
 * are externaly defined.
 * 
 * Usage example, using the defined sprite in pixmap.h:
 * <pre>
 *   #include "pixmap.h" // defines  pic1, pic2, etc 
 *   int wd, hg;
 *   char *sprite = read_xpm(pic1, &wd, &hg);
 * </pre>
*/
char *read_xpm(char *map[], int *width, int *height);

/** Creates with 0 speed and position, a new sprite with pixmap "pic", in
 * memory whose address is "base".
 * Returns NULL on invalid pixmap.
 */
Sprite* create_sprite(char *pic[]);
Sprite* create_unbuffered_sprite(char *pic[]);
Sprite* capture_screen(int x_ini, int y_ini, int width, int height, char* base);

/** draws a sprite in graphics memory
* the sprite's background color isn't drawn 
*/

Bool draw_ball(Sprite* sprt, char *base);
void draw_sprite(Sprite *sprt, char *base);
void update_sprite(Sprite* sprt, char* base);


/** Removes the sprite from the screen */
void delete_sprite(Sprite *sprt, char *base);



/** The "sprt" sprite is erased from memory whose address is "base"
 * and used resources released.
 */
void destroy_sprite(Sprite *sprt);



/** @} end of sprite */

#endif
